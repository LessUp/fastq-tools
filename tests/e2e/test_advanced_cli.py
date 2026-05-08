import subprocess
import os
import unittest
import tempfile
import shutil
import gzip

class TestFastQToolsCLI(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # Path to the executable
        cls.fastqtools = os.environ.get("FASTQTOOLS", "./build/clang-release/FastQTools")
        cls.data_dir = os.path.join(os.getcwd(), "tools/data")
        cls.sample_fastq = os.path.join(cls.data_dir, "sample_10k_len100.fastq")
        
        if not os.path.exists(cls.fastqtools):
            raise unittest.SkipTest(f"Executable not found at {cls.fastqtools}")
        if not os.path.exists(cls.sample_fastq):
            raise unittest.SkipTest(f"Sample data not found at {cls.sample_fastq}")

    def setUp(self):
        self.test_dir = tempfile.mkdtemp()

    def tearDown(self):
        shutil.rmtree(self.test_dir)

    def run_cmd(self, args):
        cmd = [self.fastqtools] + args
        result = subprocess.run(cmd, capture_output=True, text=True)
        return result

    @staticmethod
    def _is_gzip(filepath):
        """Check if a file is gzip-compressed by reading its magic bytes."""
        with open(filepath, 'rb') as f:
            return f.read(2) == b'\x1f\x8b'

    @classmethod
    def _read_fastq_lines(cls, filepath):
        """Read lines from a FASTQ file, auto-detecting gzip compression."""
        if cls._is_gzip(filepath):
            with gzip.open(filepath, 'rt') as f:
                return f.readlines()
        else:
            with open(filepath, 'r') as f:
                return f.readlines()

    @classmethod
    def _read_fastq_content(cls, filepath):
        """Read full content from a FASTQ file, auto-detecting gzip compression."""
        if cls._is_gzip(filepath):
            with gzip.open(filepath, 'rt') as f:
                return f.read()
        else:
            with open(filepath, 'r') as f:
                return f.read()

    def test_global_help(self):
        result = self.run_cmd(["--help"])
        self.assertEqual(result.returncode, 0)
        self.assertIn("Available commands", result.stdout)

    def test_filter_help(self):
        result = self.run_cmd(["filter", "--help"])
        self.assertEqual(result.returncode, 0)
        self.assertIn("Usage:", result.stdout)
        self.assertIn("--input", result.stdout)
        self.assertIn("--output", result.stdout)

    def test_stat_help(self):
        result = self.run_cmd(["stat", "--help"])
        self.assertEqual(result.returncode, 0)
        self.assertIn("Usage:", result.stdout)

    def test_filter_basic(self):
        output_fastq = os.path.join(self.test_dir, "output.fastq")
        result = self.run_cmd(["filter", "--input", self.sample_fastq, "--output", output_fastq, "--threads", "2"])
        
        self.assertEqual(result.returncode, 0)
        self.assertTrue(os.path.exists(output_fastq))
        
        # Count reads in output (each read is 4 lines in FASTQ)
        lines = self._read_fastq_lines(output_fastq)
        self.assertEqual(len(lines), 40000) # 10k reads * 4 lines

    def test_stat_basic(self):
        output_stats = os.path.join(self.test_dir, "stats.txt")
        result = self.run_cmd(["stat", "--input", self.sample_fastq, "--output", output_stats, "--threads", "2"])
        
        self.assertEqual(result.returncode, 0)
        self.assertTrue(os.path.exists(output_stats))
        
        with open(output_stats, 'r') as f:
            content = f.read()
            self.assertIn("#ReadNum\t10000", content)
            self.assertIn("#BaseCount\t1000000", content)
            self.assertIn("#MaxReadLength\t100", content)

    def test_filter_min_length(self):
        # Our sample data has all 100bp reads. Filter for min 101bp should result in 0 reads.
        output_fastq = os.path.join(self.test_dir, "empty.fastq")
        result = self.run_cmd(["filter", "--input", self.sample_fastq, "--output", output_fastq, "--min-length", "101"])
        
        self.assertEqual(result.returncode, 0)
        self.assertTrue(os.path.exists(output_fastq))
        self.assertEqual(self._read_fastq_content(output_fastq), "")

    def test_filter_max_length(self):
        # Filter for max 99bp should result in 0 reads.
        output_fastq = os.path.join(self.test_dir, "empty.fastq")
        result = self.run_cmd(["filter", "--input", self.sample_fastq, "--output", output_fastq, "--max-length", "99"])

        self.assertEqual(result.returncode, 0)
        self.assertTrue(os.path.exists(output_fastq))
        self.assertEqual(self._read_fastq_content(output_fastq), "")

    def test_filter_trim_quality_trims_low_quality_ends(self):
        input_fastq = os.path.join(self.test_dir, "trim_input.fastq")
        output_fastq = os.path.join(self.test_dir, "trimmed.fastq")
        with open(input_fastq, 'w') as f:
            f.write("@read1\nACGT\n+\n!!II\n")

        result = self.run_cmd([
            "filter",
            "--input", input_fastq,
            "--output", output_fastq,
            "--trim-quality", "20",
        ])

        self.assertEqual(result.returncode, 0)
        self.assertTrue(os.path.exists(output_fastq))
        content = self._read_fastq_content(output_fastq)
        self.assertIn("\nGT\n+\nII\n", content)

    def test_filter_accepts_profile_option(self):
        output_fastq = os.path.join(self.test_dir, "profile.fastq")
        result = self.run_cmd([
            "filter",
            "--input", self.sample_fastq,
            "--output", output_fastq,
            "--threads", "2",
            "--profile", "highThroughput",
        ])

        self.assertEqual(result.returncode, 0)
        self.assertTrue(os.path.exists(output_fastq))

    def test_stat_accepts_profile_option(self):
        output_stats = os.path.join(self.test_dir, "profile-stats.txt")
        result = self.run_cmd([
            "stat",
            "--input", self.sample_fastq,
            "--output", output_stats,
            "--threads", "2",
            "--profile", "lowMemory",
        ])

        self.assertEqual(result.returncode, 0)
        self.assertTrue(os.path.exists(output_stats))

    def test_filter_accepts_memory_limit_option(self):
        output_fastq = os.path.join(self.test_dir, "memory-limit.fastq")
        result = self.run_cmd([
            "filter",
            "--input", self.sample_fastq,
            "--output", output_fastq,
            "--threads", "2",
            "--memory-limit-gb", "1",
        ])

        self.assertEqual(result.returncode, 0)
        self.assertTrue(os.path.exists(output_fastq))

    def test_stat_accepts_memory_limit_option(self):
        output_stats = os.path.join(self.test_dir, "memory-limit-stats.txt")
        result = self.run_cmd([
            "stat",
            "--input", self.sample_fastq,
            "--output", output_stats,
            "--threads", "2",
            "--memory-limit-gb", "1",
        ])

        self.assertEqual(result.returncode, 0)
        self.assertTrue(os.path.exists(output_stats))

    def test_filter_accepts_adapter_and_poly_tail_options(self):
        input_fastq = os.path.join(self.test_dir, "preprocess.fastq")
        output_fastq = os.path.join(self.test_dir, "preprocess.out.fastq")
        with open(input_fastq, "w") as f:
            f.write("@read1\nACGTGGGG\n+\nIIIIIIII\n")
            f.write("@read2\nTTTTTTAA\n+\nIIIIIIII\n")

        result = self.run_cmd([
            "filter",
            "--input", input_fastq,
            "--output", output_fastq,
            "--adapter-seq", "TTAA",
            "--trim-poly-g", "4",
        ])

        self.assertEqual(result.returncode, 0)
        content = self._read_fastq_content(output_fastq)
        self.assertIn("@read1\nACGT\n+\nIIII\n", content)
        self.assertIn("@read2\nTTTT\n+\nIIII\n", content)

    def test_stat_writes_signature_sidecar_when_requested(self):
        input_fastq = os.path.join(self.test_dir, "signature.fastq")
        output_stats = os.path.join(self.test_dir, "signature-stats.txt")
        output_sidecar = os.path.join(self.test_dir, "signature.tsv")
        with open(input_fastq, "w") as f:
            f.write("@read1\nACGTAAAA\n+\nIIIIIIII\n")
            f.write("@read2\nACGTAAAA\n+\nIIIIIIII\n")
            f.write("@read3\nTTTTCCCC\n+\nIIIIIIII\n")

        result = self.run_cmd([
            "stat",
            "--input", input_fastq,
            "--output", output_stats,
            "--signature-report", output_sidecar,
            "--signature-kmer-size", "4",
            "--signature-limit", "10",
            "--duplicate-sample-modulo", "1",
        ])

        self.assertEqual(result.returncode, 0)
        with open(output_stats, "r") as f:
            stats_content = f.read()
        with open(output_sidecar, "r") as f:
            sidecar_content = f.read()
        self.assertIn("#DuplicateEstimate\t1", stats_content)
        self.assertIn("summary\tduplicate_estimate\t1", sidecar_content)
        self.assertIn("head_kmer\tACGT\t2", sidecar_content)

if __name__ == "__main__":
    unittest.main()
