import test from 'node:test'
import assert from 'node:assert/strict'
import { existsSync, readFileSync } from 'node:fs'

const configSource = readFileSync(new URL('../.vitepress/config.ts', import.meta.url), 'utf8')

test('published docs exclude internal superpowers artifacts', () => {
  assert.match(configSource, /srcExclude:\s*\[[\s\S]*'superpowers\/\*\*'/)
  assert.match(configSource, /llmstxt\(\{\s*ignoreFiles:\s*\[[\s\S]*'superpowers\/\*\*'/)
})

test('tracked whitepaper planning artifacts live outside docs', () => {
  assert.equal(
    existsSync(new URL('../superpowers/plans/2026-05-17-github-pages-whitepaper-redesign.md', import.meta.url)),
    false,
  )
  assert.equal(
    existsSync(new URL('../superpowers/specs/2026-05-17-github-pages-whitepaper-redesign-design.md', import.meta.url)),
    false,
  )
  assert.equal(
    existsSync(new URL('../../development/superpowers/plans/2026-05-17-github-pages-whitepaper-redesign.md', import.meta.url)),
    true,
  )
  assert.equal(
    existsSync(new URL('../../development/superpowers/specs/2026-05-17-github-pages-whitepaper-redesign-design.md', import.meta.url)),
    true,
  )
})
