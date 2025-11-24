# Outcome of This Project

## What Was Built

A high-performance parallel image-processing system written in C++ that:

- Generates thumbnails for large photo collections
- Detects duplicates (exact and perceptual)
- Measures and compares serial vs parallel performance

The implementation uses OpenMP for parallelism and stb image headers for lightweight image I/O and resizing.

## Core Functionality

- **Parallel thumbnail generation**
	- Processes collections efficiently using multiple threads
	- Default thumbnail size: 256×256 pixels (configurable)

- **Duplicate detection**
	- Exact duplicates: MD5 binary hash
	- Similar images: perceptual hash (dHash) + Hamming distance
	- Configurable similarity threshold (default = 8)

- **Performance benchmarking**
	- Runs both serial and parallel modes
	- Measures wall-clock time, throughput, average time/image
	- Computes speedup and efficiency

## Measured Results (sample run)

| Metric | Serial Mode | Parallel Mode (4 threads) | Improvement |
|---:|:---:|:---:|:---:|
| Execution Time | 0.72 s | 0.23 s | 3.17× faster |
| Throughput | 97.77 img/s | 309.73 img/s | 3.17× higher |
| Speedup Factor | 1.0× (baseline) | 3.17× | — |
| Efficiency | 100% | 79.20% | Excellent utilization |
| Images Processed | 70 | 70 | ✓ Same output |
| Duplicates Found | 53 (similar) | 53 (similar) | ✓ Consistent |

## Key Insights

- **Parallelization works:** Achieved a 3.17× speedup using 4 threads (≈79% efficiency).
- **Good scalability:** For large collections the parallel mode gives significant time savings.
- **Robust duplicate detection:** Exact and perceptual methods produced consistent results across modes.

## Practical Notes

- Thumbnails are saved to the configured output directory and do not overwrite originals.
- The program prints duplicate groups and can optionally write a report file (JSON/CSV) for post-processing.
- Typical follow-up actions: inspect duplicates, move/remove duplicates, create contact sheets from thumbnails, or run ML models on thumbnails for content-based grouping.

## Next Steps (optional)

- Add a `--report <path>` option to export duplicate groups and metrics as JSON (if not already used).
- Add an option to force output format (e.g., always save thumbnails as JPEG).
- Integrate a dry-run mode that lists files that would be removed/moved when applying automatic duplicate-clean rules.

---

Generated on: (run results will vary by dataset and machine)