import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const pages = {
  'zh/orientation/index.md': {
    required: ['# 导读', '## 阅读路径', '## 术语表'],
  },
  'zh/whitepaper/index.md': {
    required: ['# 技术白皮书', '## 系统全景', '## 阅读建议'],
  },
  'zh/why-fastqtools/index.md': {
    required: ['## 适用场景', '## 与同类方案的差异', '## 不适合谁'],
  },
  'zh/architecture/index.md': {
    required: ['## 系统分层', '## 执行模型', '## 关键权衡', '`FastqBatch`', 'source → processing → sink'],
  },
  'zh/performance/index.md': {
    required: [
      '## 证据摘要',
      '## benchmark 如何解读',
      '## 风险边界',
      '100K reads、150 bp、AMD Ryzen 9 5900X、Release 构建',
      '叙事层 → 结果层 → 方法层 → 规范层',
      '压缩比例',
      '存储 I/O',
      '线程数',
      '输入分布',
      '机器拓扑',
    ],
  },
}

test('chinese narrative pages expose the approved section skeletons', () => {
  for (const [file, { required }] of Object.entries(pages)) {
    const source = readFileSync(new URL(`../${file}`, import.meta.url), 'utf8')
    for (const heading of required) {
      assert.match(source, new RegExp(heading.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
    }
  }
})

test('chinese academy and research pages expose the approved section skeletons', () => {
  const academy = readFileSync(new URL('../zh/academy/index.md', import.meta.url), 'utf8')
  const research = readFileSync(new URL('../zh/research/index.md', import.meta.url), 'utf8')
  const reference = readFileSync(new URL('../zh/reference/index.md', import.meta.url), 'utf8')

  assert.match(academy, /# 学院/)
  assert.match(academy, /## 学习路径/)
  assert.match(research, /# 研究附录/)
  assert.match(research, /## 参考文献/)
  assert.match(reference, /# 参考导航/)
  assert.match(reference, /## CLI 与 API/)
})
