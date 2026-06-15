# QRS Core Validation Path Model

This is a benchmark model, not Bitcoin Core consensus integration.

The model times these buckets separately:

- structural checks: witness shape, signature length, public-key length,
  control-block length, low-bit rule, leaf-version rule, and modeled Merkle
  root reconstruction;
- qrs_msg construction: modeled message construction over deterministic fixture
  fields;
- verifier valid call: native OpenSSL SLH-DSA-SHA2-128s verification;
- total modeled valid path: structural checks, qrs_msg construction, and
  verifier call;
- invalid structural reject: fails before the verifier;
- invalid fixed-length reject: reaches the verifier and fails cryptographically.

The final Bitcoin Core validation-path benchmark remains a Draft-stage blocker.
This model exists to keep structural rejects and cryptographic rejects from
being collapsed into one number.
