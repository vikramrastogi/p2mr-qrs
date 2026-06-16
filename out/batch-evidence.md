# Batch Schnorr Baseline Evidence

- `status`: unavailable
- `reason`: Bundled upstream bitcoin-core/secp256k1 commit aea86bc3509f3ceb5c047fe6567bf8910f72a728 exposes no public BIP-340 batch verification API; local schnorrsig tests still mark verify_batch as TODO; and the upstream head search found no batch/Schnorr/BIP-340 branch to wire as a reviewed experimental baseline.
- `bundled_commit_expected`: aea86bc3509f3ceb5c047fe6567bf8910f72a728
- `bundled_commit_actual`: vendored source has no .git metadata; expected commit recorded in third_party/secp256k1.COMMIT
- `public_batch_api_detected`: false
- `verify_batch_todo_mentions`: 3
- `upstream_head_search.status`: ok
- `upstream_head_search.timeout_seconds`: 15.0
- `upstream_head_search.matching_heads`: 0
- `policy`: No batch Schnorr timing is synthesized from assumed speedups.

## Public Schnorr APIs

- `secp256k1_schnorrsig_extraparams`
- `secp256k1_schnorrsig_sign`
- `secp256k1_schnorrsig_sign32`
- `secp256k1_schnorrsig_sign_custom`
- `secp256k1_schnorrsig_verify`

## verify_batch TODO Mentions

- `third_party/secp256k1/src/modules/schnorrsig/tests_impl.h:200`: * Checks that both verify and verify_batch (TODO) return the same value as expected. */
- `third_party/secp256k1/src/modules/schnorrsig/tests_impl.h:880`: * verify_batch (TODO). Then flips some bits and checks that verification now
- `third_party/secp256k1/src/modules/schnorrsig/tests_impl.h:903`: * verify and verify_batch (TODO) fail */

## Upstream Matching Heads

- none
