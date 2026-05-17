import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const pages = {
  'zh/orientation/index.md': ['# 导读', '## 阅读路径', '## 术语表'],
  'zh/whitepaper/index.md': ['# 技术白皮书', '## 系统全景', '## 阅读建议'],
  'zh/why-fastqtools/index.md': ['## 适用场景', '## 与同类方案的差异', '## 不适合谁'],
  'zh/architecture/index.md': ['## 系统分层', '## 执行模型', '## 关键权衡', '`FastqBatch`', 'source → processing → sink'],
  'zh/performance/index.md': ['## 证据摘要', '## benchmark 如何解读', '## 风险边界'],
}

test('chinese narrative pages expose the approved section skeletons', () => {
  for (const [file, headings] of Object.entries(pages)) {
    const source = readFileSync(new URL(`../${file}`, import.meta.url), 'utf8')
    for (const heading of headings) {
      assert.match(source, new RegExp(heading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }
  }
})
