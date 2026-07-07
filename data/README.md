# data/

Cached external reference data, each extract recorded with its query, date, and
SHA-256 (ARCHITECTURE.md §4): Odlyzko zero tables (`data/odlyzko/`, Stage 5) and
LMFDB extracts (cubic field counts, Stage 6). Tests read only the local cache;
they never fetch at test time.

Added with the stages that consume them. Empty at Stage 0.
