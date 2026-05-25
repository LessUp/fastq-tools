import test from 'node:test'
import assert from 'node:assert/strict'
import { existsSync, readFileSync } from 'node:fs'

const configSource = readFileSync(new URL('../.vitepress/config.ts', import.meta.url), 'utf8')
const packageSource = readFileSync(new URL('../package.json', import.meta.url), 'utf8')
const enArchiveSource = readFileSync(new URL('../en/archive/index.md', import.meta.url), 'utf8')
const zhArchiveSource = readFileSync(new URL('../zh/archive/index.md', import.meta.url), 'utf8')

test('published docs exclude internal superpowers artifacts', () => {
  assert.match(configSource, /srcExclude:\s*\[[\s\S]*'superpowers\/\*\*'/)
  assert.match(configSource, /llmstxt\(\{\s*ignoreFiles:\s*\[[\s\S]*'superpowers\/\*\*'/)
})

test('published docs exclude internal asset maintenance readmes', () => {
  assert.equal(existsSync(new URL('../assets/images/README.md', import.meta.url)), true)
  assert.equal(existsSync(new URL('../assets/diagrams/README.md', import.meta.url)), true)
  assert.match(configSource, /srcExclude:\s*\[[\s\S]*'assets\/\*\*\/README\.md'/)
  assert.match(configSource, /llmstxt\(\{\s*ignoreFiles:\s*\[[\s\S]*'assets\/\*\*\/README\.md'/)
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

test('docs package no longer syncs changelog content into publication builds', () => {
  assert.doesNotMatch(packageSource, /"sync"\s*:/)
  assert.doesNotMatch(packageSource, /sync-changelog\.mjs/)
  assert.doesNotMatch(packageSource, /npm run sync && vitepress (dev|build)/)
})

test('docs publication does not ship generated changelog pages', () => {
  for (const relativePath of [
    '../en/release-notes/changelog.md',
    '../zh/release-notes/changelog.md',
    '../en/changelog.md',
    '../zh/changelog.md',
    '../scripts/sync-changelog.mjs',
  ]) {
    assert.equal(existsSync(new URL(relativePath, import.meta.url)), false, `${relativePath} should be removed`)
  }
})

test('vitepress build does not suppress deleted changelog dead links globally', () => {
  assert.doesNotMatch(configSource, /ignoreDeadLinks:\s*\[[\s\S]*release-notes\\\/changelog/)
})

test('archive indexes point changelog readers to external history surfaces', () => {
  for (const archiveSource of [enArchiveSource, zhArchiveSource]) {
    assert.doesNotMatch(archiveSource, /\.\.\/release-notes\/changelog/)
    assert.match(archiveSource, /github\.com\/LessUp\/fastq-tools\/blob\/master\/CHANGELOG\.md/)
    assert.match(archiveSource, /github\.com\/LessUp\/fastq-tools\/releases/)
  }
})
