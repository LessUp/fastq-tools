import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

test('english mirror pages expose the approved section skeletons', () => {
  const pages = {
    'en/orientation/index.md': ['# Orientation', '## Reading path', '## Terms'],
    'en/whitepaper/index.md': ['# Whitepaper', '## System overview', '## Reading path'],
    'en/algorithms/index.md': ['# Algorithms', '## Filtering pipeline'],
    'en/research/index.md': ['# Research appendix', '## References', '## Design evolution'],
  }

  for (const [file, headings] of Object.entries(pages)) {
    const source = readFileSync(new URL(`../${file}`, import.meta.url), 'utf8')
    for (const heading of headings) {
      assert.match(source, new RegExp(heading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }
  }
})

test('english command examples preserve the approved multiline layout', () => {
  const gettingStarted = readFileSync(
    new URL('../en/guide/getting-started.md', import.meta.url),
    'utf8'
  )
  const cliReference = readFileSync(
    new URL('../en/guide/cli-reference.md', import.meta.url),
    'utf8'
  )

  assert.match(
    gettingStarted,
    /FastQTools filter -i reads\.fastq\.gz -o filtered\.fastq\.gz \\\n\s+--min-quality 20 \\\n\s+--min-length 50 \\\n\s+--trim-quality 20 \\\n\s+--trim-mode both/
  )
  assert.match(
    cliReference,
    /FastQTools stat -i reads\.fastq\.gz -o stats\.txt \\\n\s+--signature-report signatures\.tsv \\\n\s+--signature-kmer-size 15/
  )
  assert.match(
    cliReference,
    /FastQTools filter -i reads\.fastq\.gz -o clean\.fastq\.gz \\\n\s+--min-quality 20 \\\n\s+--min-length 50 \\\n\s+--max-n-ratio 0\.1 \\\n\s+--trim-quality 20 \\\n\s+--trim-mode both/
  )
})
