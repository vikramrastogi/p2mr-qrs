# Report Generation

Exact goal:
- Generate reviewer-ready Markdown report from JSON.

Files created:
- `src/report_json.cpp`
- `src/report_markdown.cpp`
- `docs/native-benchmark-report-template.md`
- `scripts/assert_quick_report.py`

Required behavior:
- Report environment.
- Report library commits and versions.
- Report raw timing table.
- Report block model table.
- Report caveats.
- State whether the result supports existing weight accounting or suggests explicit QRS budget.
- Include no overclaiming.

Report conclusion language:
- If QRS p99 saturated block time is <= both individual and reviewed-public batch Schnorr p99 saturated block baselines:
  "On this machine and with these library versions, the result is consistent with the no-additional-budget hypothesis, subject to broader native validation-cost review."
- If QRS exceeds reviewed-public batch but not individual:
  "On this machine, QRS exceeds the batch Schnorr baseline; reviewers should decide whether an explicit QRS budget is required."
- If QRS exceeds individual:
  "On this machine, QRS exceeds the individual Schnorr baseline; this result suggests an explicit QRS validation budget is required."
- If individual Schnorr is unavailable, the report says the run is not proof-quality yet.
- If reviewed-public batch Schnorr is unavailable but experimental batch Schnorr is available, the report says the experimental batch result is sensitivity analysis, not consensus proof.
- If both reviewed-public and experimental batch Schnorr are unavailable, the report says the batch baseline does not close the resource-accounting question.

Acceptance commands:

```sh
./build/qrs_native_bench --full --json out/full.json --markdown out/full.md
grep -n "Conclusion" out/full.md
grep -n "Caveats" out/full.md
python3 scripts/assert_quick_report.py --json out/quick.json --markdown out/quick.md --batch-evidence-json out/batch-evidence.json
```
