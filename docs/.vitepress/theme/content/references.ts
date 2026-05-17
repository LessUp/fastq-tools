import type { LocaleKey, ThemeConceptId } from './siteContent'

export type ReferenceEntry = {
  id: string
  concept: ThemeConceptId
  title: Record<LocaleKey, string>
  summary: Record<LocaleKey, string>
  href?: string
  docPath?: string
}

export const referenceCatalog = [
  {
    id: 'whitepaper-overview',
    concept: 'whitepaper',
    docPath: 'whitepaper/',
    title: {
      zh: '白皮书总览',
      en: 'Whitepaper overview',
    },
    summary: {
      zh: '统一入口，串联定位、架构与证据叙事。',
      en: 'Primary narrative entry that ties positioning, architecture, and evidence together.',
    },
  },
  {
    id: 'architecture-rationale',
    concept: 'architecture',
    docPath: 'architecture/',
    title: {
      zh: '架构设计理由',
      en: 'Architecture rationale',
    },
    summary: {
      zh: '追踪零拷贝 I/O、并行流水线与可维护边界。',
      en: 'Trace zero-copy I/O, parallel pipelines, and maintainable boundaries.',
    },
  },
  {
    id: 'workflow-operator-path',
    concept: 'algorithms',
    docPath: 'academy/',
    title: {
      zh: '算法与流程入口',
      en: 'Algorithm and workflow entry',
    },
    summary: {
      zh: '把学习路径、工作流与执行页面组织到同一导航骨架里。',
      en: 'Keeps learning paths, workflows, and execution pages in one navigation skeleton.',
    },
  },
  {
    id: 'benchmark-method',
    concept: 'performance',
    docPath: 'performance/benchmark-report',
    title: {
      zh: 'Benchmark 方法说明',
      en: 'Benchmark methodology',
    },
    summary: {
      zh: '说明环境、约束和如何阅读吞吐数字。',
      en: 'Explains environment, caveats, and how to read throughput numbers.',
    },
  },
  {
    id: 'reference-hub',
    concept: 'referenceNav',
    docPath: 'reference/',
    title: {
      zh: '参考导航',
      en: 'Reference hub',
    },
    summary: {
      zh: '面向 CLI、API、构建与测试资料的统一入口。',
      en: 'Shared entry for CLI, API, build, and testing material.',
    },
  },
  {
    id: 'research-context',
    concept: 'researchNav',
    docPath: 'research/',
    title: {
      zh: '研究补充',
      en: 'Research appendix',
    },
    summary: {
      zh: '收纳参考资料、协作入口与历史上下文。',
      en: 'Collects references, collaboration entry points, and historical context.',
    },
  },
] as const satisfies readonly ReferenceEntry[]

export const referenceCollections = {
  referenceNav: ['reference-hub', 'workflow-operator-path'],
  researchNav: ['research-context', 'benchmark-method'],
} as const satisfies Record<'referenceNav' | 'researchNav', readonly string[]>
