# BIP-360 Dependency Matrix

Status: blocked on final BIP-360/P2MR definitions.

This package depends on BIP-360 semantics, but it does not finalize BIP-360.
The QRS leaf should not advance beyond Draft until each dependency below has a
stable definition and a matching vector or integration check.

| Dependency | Needed by QRS | Current package status | Closure artifact |
| --- | --- | --- | --- |
| P2MR output script form | Binding the Merkle root in `scriptPubKey` | Modeled as `OP_1 <32-byte-root>` in provisional fixtures | Final BIP-360 output definition and serialized vectors |
| future leaf version behavior | Assigning one non-tapscript leaf version to QRS | Draft uses one P2MR-only future leaf version | Final BIP-360 future-leaf semantics and conflict review |
| zero-depth / single-leaf P2MR tree behavior | Avoiding accidental dependence on depth-zero tree semantics | Draft witness model permits `m = 0`, but does not recommend relying on zero-depth outputs | Final BIP-360 position on zero-depth/single-leaf trees and regenerated vectors |
| TapLeaf/TapBranch hashing | Computing `leaf_hash` and Merkle root | Byte-level model uses TapLeaf/TapBranch structure with provisional leaf version | Final BIP-360 hash-tag review |
| SigMsg extension behavior | Binding QRS to a transaction digest with `ext_flag=2` | Byte-level model tests the QRS preimage shape only | Final QRS `ext_flag=2` review and Bitcoin Core-shaped implementation |
| Annex handling | Ensuring annex presence changes the digest | Provisional vectors cover annex and no-annex cases | Final BIP-360/QRS annex rules |
| Control-block validation | Distinguishing structural rejects from verifier-reaching spends | Fixture verifier checks length, low bit, leaf version, and Merkle root | Final BIP-360 control-block rules |
| Serialized transaction vectors | Cross-implementation digest agreement | Current model has Python/native agreement over provisional JSON fixtures | Final serialized valid and invalid transactions |

## Review Rule

If BIP-360 changes any output, leaf, or digest rule that affects this matrix,
the QRS vectors and BIP text must be regenerated before the package is cited as
current pre-review evidence.
