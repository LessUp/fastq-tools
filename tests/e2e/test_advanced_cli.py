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
        cls.fastqtools = os.environ.get("FASTQTOOLS", "./build-clang-release/FastQTools")
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
        with gzip.open(output_fastq, 'rt') as f:
            lines = f.readlines()
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
        with gzip.open(output_fastq, 'rt') as f:
            self.assertEqual(f.read(), "")

    def test_filter_max_length(self):
        # Filter for max 99bp should result in 0 reads.
        output_fastq = os.path.join(self.test_dir, "empty.fastq")
        result = self.run_cmd(["filter", "--input", self.sample_fastq, "--output", output_fastq, "--max-length", "99"])
        
        self.assertEqual(result.returncode, 0)
        self.assertTrue(os.path.exists(output_fastq))
        with gzip.open(output_fastq, 'rt') as f:
            self.assertEqual(f.read(), "")

if __name__ == "__main__":
    unittest.main()
