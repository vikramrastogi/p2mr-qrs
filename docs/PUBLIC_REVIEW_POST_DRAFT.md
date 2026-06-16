# Public Review Post Draft

I am sharing a Draft-stage pre-review package for a narrow P2MR SLH-DSA leaf.

This is not activation-ready. It is not a replacement for BIP-360 or BIP-361,
and it does not propose legacy-output policy. The draft defines one P2MR-only
future leaf version whose payload is one SLH-DSA-SHA2-128s public key and whose
spend verifies one fixed-size SLH-DSA-SHA2-128s signature.

The proposal intentionally does not add a tapscript opcode, witness discount,
algorithm registry, activation parameters, or old-output freeze/burn/throttle/
sunset/rescue rule.

The main review question is resource accounting. The package includes native
OpenSSL SLH-DSA timing, native individual BIP-340 Schnorr timing, experimental
batch-Schnorr sensitivity, reviewed-public batch-Schnorr availability evidence,
draft cryptographic vectors, and a byte-level provisional QRS digest model with
Python/native agreement. The QRS digest uses `ext_flag=2` in the provisional
model, but final consensus vectors remain blocked on final BIP-360/QRS hashing
and Bitcoin Core-shaped SigMsg definitions.

Known blockers before advancing beyond Draft include final BIP-360 dependency
review, final QRS SigMsg review, Bitcoin Core consensus integration,
cross-hardware reproduction, second reviewed SLH-DSA verifier strategy, and
reviewer agreement on resource accounting and batch-Schnorr sensitivity.
