import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

test('english mirror pages expose the approved section skeletons', () => {
  const pages = {
    'en/orientation/index.md': ['# Orientation', '## Reading path', '## Terms'],
    'en/whitepaper/index.md': ['# Whitepaper', '## System overview', '## Reading path'],
    'en/academy/index.md': ['# Academy', '## Learning tracks'],
    'en/research/index.md': ['# Research appendix', '## References', '## Design evolution'],
  }

  for (const [file, headings] of Object.entries(pages)) {
    const source = readFileSync(new URL(`../${file}`, import.meta.url), 'utf8')
    for (const heading of headings) {
      assert.match(source, new RegExp(heading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }
  }
})
