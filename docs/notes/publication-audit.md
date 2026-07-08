# Publication audit — git history is free of copyrighted / large blobs

Evidence, not assurance. All commands below were run against a **fresh clone of
the public remote** `github.com/DerekMarshall/primeknots` (not the local working
copy), after the `filter-branch` purge + force-push. Raw output is pasted verbatim.

    git clone https://github.com/DerekMarshall/primeknots.git pk-audit

- **HEAD:** `f1d30fd4520c6b6b49133a99e598e3f9cb539472`
- **commit count:** 53
- **audit date:** 2026-07-08

## Check 1 — no commit ever added a paper PDF

```
$ git log --all --diff-filter=A --oneline -- 'docs/papers/*.pdf'
                       (no output — zero PDF-add commits)
```

The only add under `docs/papers/` in all history is the references stub, not a PDF:

```
$ git log --all --diff-filter=A --oneline -- docs/papers/
a7cfb5d chore: prep for public remote + Stage 5 Odlyzko oracle   # added docs/papers/README.md
```

## Check 2 — no `.pdf` blob is reachable anywhere in history

```
$ git rev-list --objects --all | grep -iE '\.pdf$'
                       (no output — zero PDF blobs)
```

## Check 3 — no blob exceeds 1 MB; every survivor is justified

```
$ git rev-list --objects --all \
    | git cat-file --batch-check='%(objecttype) %(objectname) %(objectsize) %(rest)' \
    | awk '$1=="blob" && ($3+0)>1048576 {print $3, $4}' | sort -rn
                       (no output — no blob exceeds 1 MB)
```

Largest six blobs in the entire history, all well under 1 MB, each justified:

| size (bytes) | path | justification |
|---|---|---|
| 323200 | `third_party/doctest/doctest.h` | vendored test framework (MIT), single header — required, small |
| 191937 | `docs/img/borromean.png` | README gallery panel — our own viewer screenshot of emitted JSON |
| 87706 | `docs/img/classgroups.png` | README gallery panel — ditto |
| 87494 | `docs/img/explicit_formula.png` | README gallery panel — ditto |
| 53059 | `docs/img/linking.png` | README gallery panel — ditto |
| 22492 | `docs/notes/stage6-pinning.md` | pinning log (text) |

## Supplementary — the shipped-mechanism invariants hold

The Odlyzko zero table (~1.8 MB) is **not** in history — only its checksum
manifest is committed; the raw table is fetched by `oracle/fetch_odlyzko.py`:

```
$ git rev-list --objects --all | grep -iE 'odlyzko/zeros1'
                       (no output — raw table absent)

$ git ls-tree -r --name-only HEAD | grep -E '^(docs/papers|data)/'
data/README.md
data/belabas/cubic_s3.txt          # small referee-count cache (text)
data/cubic/s3_counts.txt           # small referee-count cache (text)
data/odlyzko/MANIFEST.json         # checksum manifest, NOT the table
docs/papers/README.md              # references stub, NOT the papers
```

No build artifacts or emitted visualization data are in history:

```
$ git rev-list --objects --all | grep -iE '(^|/)(build/|viz/data/|\.o$|\.a$|\.dylib$)'
                       (no output — none)
```

## Conclusion

The purge is verified from the remote by direct enumeration: zero copyrighted
PDFs, zero blobs over 1 MB, the Odlyzko table shipped only as fetch-script +
checksum manifest (as designed), and no build/emit artifacts. Nothing found; no
further history rewrite required.
