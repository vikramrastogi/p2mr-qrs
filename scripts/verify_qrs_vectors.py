#!/usr/bin/env python3
"""Execute QRS vector validation, including native SLH-DSA checks.

The vectors remain provisional until BIP-360/QRS hashing is final. This script
checks the draft cryptographic vectors: it recomputes the modeled leaf/message
hashes, checks structural failure staging, and verifies real fixed-length
SLH-DSA signatures through the native benchmark binary.
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

from verify_qrs_fixtures import (  # type: ignore
    STRUCTURAL_FAILURES,
    classify,
    expand_descriptor,
    hex_bytes,
    merkle_root,
    modeled_leaf_hash,
    modeled_qrs_msg,
    update_spend_root,
)


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_BINARY = ROOT / "build" / "qrs_native_bench"
CRYPTO_STAGES = {"none", "slh_dsa_verify"}


class VectorError(AssertionError):
    pass


def fail(path: Path, message: str) -> None:
    raise VectorError(f"{path}: {message}")


def is_hex_string(value: object) -> bool:
    return isinstance(value, str) and re.fullmatch(r"[0-9a-f]*", value) is not None and len(value) % 2 == 0


def load_vectors(vectors_dir: Path) -> list[tuple[Path, dict]]:
    paths = sorted(
        path for path in vectors_dir.glob("qrs_*.json") if not path.name.endswith(".schema.json")
    )
    if not paths:
        raise VectorError(f"no qrs_*.json vectors found under {vectors_dir}")
    return [(path, json.loads(path.read_text(encoding="utf-8"))) for path in paths]


def run_native(binary: Path, args: list[str]) -> tuple[str, str]:
    proc = subprocess.run(
        [str(binary), *args],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    return proc.stdout.strip(), proc.stderr.strip() if proc.returncode else ""


def native_available(binary: Path) -> bool:
    if not binary.exists():
        return False
    stdout, stderr = run_native(binary, ["--slh-verify", "--public-key-hex", "00" * 32, "--message-hex", "00" * 32, "--signature-hex", "00" * 7856])
    return stdout in {"valid", "invalid"} and not stderr


def native_verify(binary: Path, pubkey_hex: str, msg_hex: str, sig_hex: str, path: Path) -> str:
    stdout, stderr = run_native(
        binary,
        [
            "--slh-verify",
            "--public-key-hex",
            pubkey_hex,
            "--message-hex",
            msg_hex,
            "--signature-hex",
            sig_hex,
        ],
    )
    if stderr:
        fail(path, f"native SLH-DSA verifier unavailable or failed: {stderr}")
    if stdout not in {"valid", "invalid"}:
        fail(path, f"native SLH-DSA verifier returned unexpected output {stdout!r}")
    return stdout


def native_keygen(binary: Path) -> dict:
    stdout, stderr = run_native(binary, ["--slh-keygen"])
    if stderr:
        raise VectorError(f"native SLH-DSA keygen failed: {stderr}")
    return json.loads(stdout)


def native_sign(binary: Path, private_key_hex: str, msg_hex: str) -> str:
    stdout, stderr = run_native(
        binary,
        ["--slh-sign", "--private-key-hex", private_key_hex, "--message-hex", msg_hex],
    )
    if stderr:
        raise VectorError(f"native SLH-DSA signing failed: {stderr}")
    if not is_hex_string(stdout) or len(stdout) != 7856 * 2:
        raise VectorError("native SLH-DSA signing returned an invalid signature encoding")
    return stdout


def mutate_signature(sig_hex: str) -> str:
    sig = bytearray(bytes.fromhex(sig_hex))
    sig[len(sig) // 2] ^= 0x01
    return sig.hex()


def verify_model(path: Path, data: dict) -> tuple[str, str, str, str]:
    sig = expand_descriptor(data["qrs_signature"], path, "qrs_signature")
    pubkey = expand_descriptor(data["qrs_public_key"], path, "qrs_public_key")
    control_block = expand_descriptor(data["control_block"], path, "control_block")
    annex = hex_bytes(data.get("annex", ""), path, "annex")

    leaf_hash = modeled_leaf_hash(pubkey)
    qrs_msg = modeled_qrs_msg(data, leaf_hash, annex, path)
    if data.get("expected_leaf_hash") != leaf_hash.hex():
        fail(path, "expected_leaf_hash does not match executable vector model")
    if data.get("expected_qrs_msg") != qrs_msg.hex():
        fail(path, "expected_qrs_msg does not match executable vector model")

    stage = classify(data, sig, pubkey, control_block, leaf_hash, path)
    if stage != data.get("failure_stage"):
        fail(path, f"classified failure_stage {stage!r}, expected {data.get('failure_stage')!r}")
    reaches = stage in CRYPTO_STAGES
    if bool(data.get("reaches_slh_dsa_verifier")) != reaches:
        fail(path, f"reaches_slh_dsa_verifier should be {reaches}")
    if stage in STRUCTURAL_FAILURES and reaches:
        fail(path, "structural failure must not reach SLH-DSA verifier")
    return stage, leaf_hash.hex(), qrs_msg.hex(), sig.hex()


def verify_crypto_vector(path: Path, data: dict, stage: str, binary: Path) -> None:
    if not is_hex_string(data["qrs_public_key"]):
        fail(path, "verifier-reaching vectors must contain a real hex qrs_public_key")
    if not is_hex_string(data["qrs_signature"]):
        fail(path, "verifier-reaching vectors must contain a real hex qrs_signature")
    pubkey_hex = data["qrs_public_key"]
    sig_hex = data["qrs_signature"]
    if len(pubkey_hex) != 64:
        fail(path, "verifier-reaching qrs_public_key must be 32 bytes")
    if len(sig_hex) != 7856 * 2:
        fail(path, "verifier-reaching qrs_signature must be 7,856 bytes")
    result = native_verify(binary, pubkey_hex, data["expected_qrs_msg"], sig_hex, path)
    if stage == "none" and result != "valid":
        fail(path, "valid vector reached SLH-DSA verifier but failed")
    if stage == "slh_dsa_verify" and result != "invalid":
        fail(path, "invalid fixed-length vector reached SLH-DSA verifier but did not fail")


def update_real_signatures(vectors: list[tuple[Path, dict]], binary: Path) -> None:
    key = native_keygen(binary)
    public_key_hex = key["public_key_hex"]
    private_key_hex = key["private_key_hex"]
    if len(public_key_hex) != 64:
        raise VectorError("native keygen did not return a 32-byte public key")

    for path, data in vectors:
        stage = data["failure_stage"]
        if stage in CRYPTO_STAGES:
            data["qrs_public_key"] = public_key_hex
            control_block = expand_descriptor(data["control_block"], path, "control_block")
            pubkey = bytes.fromhex(public_key_hex)
            leaf_hash = modeled_leaf_hash(pubkey)
            update_spend_root(data, merkle_root(leaf_hash, control_block))
            qrs_msg = modeled_qrs_msg(data, leaf_hash, hex_bytes(data.get("annex", ""), path, "annex"), path)
            sig_hex = native_sign(binary, private_key_hex, qrs_msg.hex())
            if stage == "slh_dsa_verify":
                sig_hex = mutate_signature(sig_hex)
            data["qrs_signature"] = sig_hex
            data["expected_leaf_hash"] = leaf_hash.hex()
            data["expected_qrs_msg"] = qrs_msg.hex()
        else:
            sig = expand_descriptor(data["qrs_signature"], path, "qrs_signature")
            pubkey = expand_descriptor(data["qrs_public_key"], path, "qrs_public_key")
            leaf_hash = modeled_leaf_hash(pubkey)
            qrs_msg = modeled_qrs_msg(data, leaf_hash, hex_bytes(data.get("annex", ""), path, "annex"), path)
            data["expected_leaf_hash"] = leaf_hash.hex()
            data["expected_qrs_msg"] = qrs_msg.hex()
            data["qrs_signature"] = data["qrs_signature"] if sig else data["qrs_signature"]
        path.write_text(json.dumps(data, indent=2, sort_keys=False) + "\n", encoding="utf-8")


def run_self_tests(binary: Path, crypto_checked: bool, verified: list[tuple[Path, dict, str]]) -> None:
    first_valid = next(((p, d) for p, d, s in verified if s == "none"), None)
    if first_valid is None:
        raise VectorError("self-test requires at least one valid vector")

    path, data = first_valid
    corrupted = dict(data)
    corrupted["expected_qrs_msg"] = "00" * 32
    try:
        verify_model(path, corrupted)
    except VectorError:
        pass
    else:
        raise VectorError("self-test failed: corrupted expected_qrs_msg was accepted")

    if crypto_checked:
        bad_sig = mutate_signature(data["qrs_signature"])
        result = native_verify(binary, data["qrs_public_key"], data["expected_qrs_msg"], bad_sig, path)
        if result != "invalid":
            raise VectorError("self-test failed: corrupted valid signature verified")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("vectors", type=Path)
    parser.add_argument("--binary", type=Path, default=DEFAULT_BINARY)
    parser.add_argument("--require-crypto", action="store_true")
    parser.add_argument("--update-real-signatures", action="store_true")
    args = parser.parse_args()

    vectors = load_vectors(args.vectors)
    if args.update_real_signatures:
        if not native_available(args.binary):
            raise VectorError("native SLH-DSA backend is required to update real signatures")
        update_real_signatures(vectors, args.binary)
        vectors = load_vectors(args.vectors)

    crypto_available = native_available(args.binary)
    if args.require_crypto and not crypto_available:
        raise VectorError(f"native SLH-DSA verifier is unavailable at {args.binary}")

    verified: list[tuple[Path, dict, str]] = []
    crypto_vectors = 0
    structural_vectors = 0
    for path, data in vectors:
        stage, _, _, _ = verify_model(path, data)
        verified.append((path, data, stage))
        if stage in CRYPTO_STAGES:
            crypto_vectors += 1
            if crypto_available:
                verify_crypto_vector(path, data, stage, args.binary)
            elif is_hex_string(data["qrs_signature"]) and args.require_crypto:
                fail(path, "native crypto verification required but unavailable")
        else:
            structural_vectors += 1

    run_self_tests(args.binary, crypto_available, verified)
    suffix = "with native SLH-DSA crypto" if crypto_available else "without native SLH-DSA crypto"
    print(
        f"verified {len(verified)} draft cryptographic QRS vectors "
        f"({crypto_vectors} verifier-reaching, {structural_vectors} structural) {suffix}"
    )
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (VectorError, AssertionError, subprocess.SubprocessError, json.JSONDecodeError) as exc:
        print(f"verify_qrs_vectors.py: {exc}", file=sys.stderr)
        sys.exit(1)
