<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { referenceCatalog, type ReferenceEntry } from '../content/references'
import type { LocaleKey } from '../content/siteContent'
import ReferenceBadge from './ReferenceBadge.vue'

type ReferenceVariant = 'site' | 'bibliography'

type RenderEntry = {
  label: string
  kind: string
  href?: string
  docPath?: string
  citation: string
  note: string
}

type RenderGroup = {
  title: string
  summary: string
  entries: RenderEntry[]
}

const props = withDefaults(defineProps<{
  locale: LocaleKey
  variant?: ReferenceVariant
}>(), {
  variant: 'bibliography',
})

const mapSiteEntry = (entry: ReferenceEntry, locale: LocaleKey): RenderEntry => ({
  label: entry.title[locale],
  kind: 'Doc',
  href: entry.href,
  docPath: entry.docPath,
  citation: entry.summary[locale],
  note: locale === 'zh'
    ? `主题概念：${entry.concept}`
    : `Theme concept: ${entry.concept}`,
})

const siteGroups: Record<LocaleKey, RenderGroup[]> = {
  en: [
    {
      title: 'Internal documentation anchors',
      summary:
        'These entries come from the site-wide reference catalog created in the IA foundation. Use them as the canonical internal reading trail before you jump to external context.',
      entries: referenceCatalog.map((entry) => mapSiteEntry(entry, 'en')),
    },
  ],
  zh: [
    {
      title: '站内文档锚点',
      summary:
        '这些条目直接来自信息架构阶段建立的站内参考目录，可作为进入外部资料之前的主阅读链路。',
      entries: referenceCatalog.map((entry) => mapSiteEntry(entry, 'zh')),
    },
  ],
}

const bibliographyGroups: Record<LocaleKey, RenderGroup[]> = {
  en: [
    {
      title: 'FASTQ and quality-score background',
      summary: 'Use these sources to ground format assumptions and quality-score terminology before comparing tools.',
      entries: [
        {
          label: 'Cock et al. 2010 — The Sanger FASTQ file format',
          kind: 'Paper',
          href: 'https://pmc.ncbi.nlm.nih.gov/articles/PMC2847217/',
          citation:
            'Primary background on FASTQ structure, variant history, and quality-score encoding assumptions.',
          note: 'Useful whenever documentation needs to distinguish record structure from downstream QC policy.',
        },
        {
          label: 'FastQTools Product Specification',
          kind: 'Spec',
          href: 'https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/product/fastq-processing.md',
          citation:
            'Maintained scope for stat/filter capabilities, supported predicates, mutators, and representative performance targets.',
          note: 'This is the boundary document for what the docs may claim today.',
        },
      ],
    },
    {
      title: 'Execution, memory, and evidence policy',
      summary: 'These references explain why throughput claims in the whitepaper are tied to architecture and benchmark maintenance.',
      entries: [
        {
          label: 'RFC-0001: Core Architecture',
          kind: 'RFC',
          href: 'https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md',
          citation:
            'Defines the layered architecture, zero-copy batch model, and oneTBB pipeline rationale.',
          note: 'Primary source for source → processing → sink and public API boundaries.',
        },
        {
          label: 'RFC-0004: Memory Pool Optimization',
          kind: 'RFC',
          href: 'https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md',
          citation:
            'Documents object-pool reuse and bounded batch lifecycle assumptions.',
          note: 'Use this when discussing memory discipline instead of speaking only in generic terms.',
        },
        {
          label: 'RFC-0003: Benchmark System',
          kind: 'RFC',
          href: 'https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md',
          citation:
            'Formalizes benchmark collection, report generation, and regression detection thresholds.',
          note: 'Explains where benchmark-report data comes from and how it is stored.',
        },
        {
          label: 'RFC-0006: Benchmark Maintenance Policy',
          kind: 'RFC',
          href: 'https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md',
          citation:
            'Separates release-facing SLA language from informational GitHub Pages benchmark publication.',
          note: 'Use this to avoid overstating benchmark snapshots as universal guarantees.',
        },
        {
          label: 'oneTBB documentation',
          kind: 'Library',
          href: 'https://www.intel.com/content/www/us/en/docs/onetbb/developer-guide-api-reference/2022-0/parallel-pipeline.html',
          citation:
            'Official documentation for parallel_pipeline and related execution primitives used in the processing path.',
          note: 'Helpful when mapping FastQTools stages to the underlying concurrency model.',
        },
      ],
    },
    {
      title: 'Adjacent QC and preprocessing tools',
      summary: 'These projects are the most useful comparison points when explaining what FastQTools is and is not trying to become.',
      entries: [
        {
          label: 'FastQC',
          kind: 'Project',
          href: 'https://www.bioinformatics.babraham.ac.uk/projects/fastqc/',
          citation: 'Report-oriented quality-control tool focused on analysis summaries and visual inspection.',
          note: 'Useful contrast when discussing report-first versus embeddable processing-first boundaries.',
        },
        {
          label: 'fastp',
          kind: 'Project',
          href: 'https://github.com/OpenGene/fastp',
          citation: 'Integrated FASTQ preprocessing tool with filtering, trimming, and report generation in one executable.',
          note: 'Strong comparison point for “all-in-one preprocessing” versus a narrower QC toolkit boundary.',
        },
        {
          label: 'Cutadapt',
          kind: 'Project',
          href: 'https://cutadapt.readthedocs.io/',
          citation: 'Adapter-focused trimming tool with a mature workflow and ecosystem footprint.',
          note: 'Relevant when explaining how FastQTools handles trimming without making adapter handling its only narrative.',
        },
        {
          label: 'seqtk',
          kind: 'Project',
          href: 'https://github.com/lh3/seqtk',
          citation: 'Compact FASTA/FASTQ command-line toolkit with Unix-style ergonomics and low ceremony.',
          note: 'Good contrast for readers who value compact utilities over a layered whitepaper + API story.',
        },
      ],
    },
  ],
  zh: [
    {
      title: 'FASTQ 与质量值背景',
      summary: '先用这些资料校准格式假设与质量值术语，再去比较工具或复述白皮书结论。',
      entries: [
        {
          label: 'Cock 等，2010：The Sanger FASTQ file format',
          kind: 'Paper',
          href: 'https://pmc.ncbi.nlm.nih.gov/articles/PMC2847217/',
          citation:
            'FASTQ 结构、历史变体与质量值编码背景的经典综述。',
          note: '当文档需要区分“记录格式”与“QC 策略”时，这篇文献尤其重要。',
        },
        {
          label: 'FastQTools 产品规范',
          kind: 'Spec',
          href: 'https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/product/fastq-processing.md',
          citation:
            '当前维护中的 stat/filter 范围、支持的 predicate / mutator，以及代表性性能目标。',
          note: '这是说明文档今天可以声称什么、不能声称什么的边界文件。',
        },
      ],
    },
    {
      title: '执行、内存与证据策略',
      summary: '这些参考资料解释为什么白皮书里的吞吐主张必须绑定到架构与 benchmark 维护政策。',
      entries: [
        {
          label: 'RFC-0001：核心架构',
          kind: 'RFC',
          href: 'https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md',
          citation:
            '定义分层架构、零拷贝批处理模型和 oneTBB 流水线选择理由。',
          note: 'source → processing → sink 与公共 API 边界的主事实来源。',
        },
        {
          label: 'RFC-0004：内存池优化',
          kind: 'RFC',
          href: 'https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md',
          citation:
            '说明对象池复用与批次生命周期受控的内存策略。',
          note: '讨论内存约束时，应该引用这里而不是只说“尽量少分配”。',
        },
        {
          label: 'RFC-0003：Benchmark 系统',
          kind: 'RFC',
          href: 'https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md',
          citation:
            '定义 benchmark 采集、报告生成和回归检测阈值。',
          note: '解释 benchmark-report 数据从哪里来、如何维护。',
        },
        {
          label: 'RFC-0006：Benchmark 维护策略',
          kind: 'RFC',
          href: 'https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md',
          citation:
            '明确发布口径 SLA 与 GitHub Pages 信息性 benchmark 之间的区别。',
          note: '避免把代表性快照误说成普适性能承诺。',
        },
        {
          label: 'oneTBB 官方文档',
          kind: 'Library',
          href: 'https://www.intel.com/content/www/us/en/docs/onetbb/developer-guide-api-reference/2022-0/parallel-pipeline.html',
          citation:
            'parallel_pipeline 及相关执行原语的官方说明。',
          note: '适合把 FastQTools 的阶段边界映射回底层并行模型时使用。',
        },
      ],
    },
    {
      title: '相关 QC / 预处理工具',
      summary: '这些项目是解释 FastQTools 的边界时最值得对照的参考点。',
      entries: [
        {
          label: 'FastQC',
          kind: 'Project',
          href: 'https://www.bioinformatics.babraham.ac.uk/projects/fastqc/',
          citation: '偏报告与可视化检查的质量控制工具。',
          note: '适合对照“报告优先”与“可嵌入处理优先”的差异。',
        },
        {
          label: 'fastp',
          kind: 'Project',
          href: 'https://github.com/OpenGene/fastp',
          citation: '把过滤、修剪与报告集中在一个可执行文件中的预处理工具。',
          note: '是说明“一体化预处理”与“聚焦 QC 工具包”差异的关键对象。',
        },
        {
          label: 'Cutadapt',
          kind: 'Project',
          href: 'https://cutadapt.readthedocs.io/',
          citation: '围绕 adapter trimming 建立成熟工作流的工具。',
          note: '有助于解释 FastQTools 的 trimming 叙事并不只围绕 adapter 展开。',
        },
        {
          label: 'seqtk',
          kind: 'Project',
          href: 'https://github.com/lh3/seqtk',
          citation: '低门槛、Unix 风格的 FASTA/FASTQ 小工具集。',
          note: '适合对照“紧凑工具”与“白皮书 + API 分层”之间的取舍。',
        },
      ],
    },
  ],
}

const groups = computed(() => props.variant === 'site' ? siteGroups[props.locale] : bibliographyGroups[props.locale])

const resolveHref = (entry: RenderEntry) => {
  if (entry.docPath) {
    return withBase(`/${props.locale}/${entry.docPath.replace(/^\/+/, '')}`)
  }

  return entry.href ?? '#'
}

const isExternal = (entry: RenderEntry) => Boolean(entry.href && /^https?:\/\//.test(entry.href))
</script>

<template>
  <div class="reference-groups">
    <section v-for="group in groups" :key="group.title" class="reference-group">
      <header class="reference-group-header">
        <h2>{{ group.title }}</h2>
        <p>{{ group.summary }}</p>
      </header>

      <ol class="reference-list">
        <li v-for="entry in group.entries" :key="entry.label" class="reference-item">
          <div class="reference-title-row">
            <ReferenceBadge :kind="entry.kind">{{ entry.kind }}</ReferenceBadge>
            <a
              class="reference-link"
              :href="resolveHref(entry)"
              :target="isExternal(entry) ? '_blank' : undefined"
              :rel="isExternal(entry) ? 'noreferrer' : undefined"
            >
              {{ entry.label }}
            </a>
          </div>
          <p class="reference-citation">{{ entry.citation }}</p>
          <p class="reference-note">{{ entry.note }}</p>
        </li>
      </ol>
    </section>
  </div>
</template>

<style scoped>
.reference-groups {
  display: grid;
  gap: 1.5rem;
  margin: 1.5rem 0 0;
}

.reference-group {
  display: grid;
  gap: 1rem;
  padding: 1.25rem;
  border: 1px solid var(--vp-c-border);
  border-radius: 1.2rem;
  background: linear-gradient(180deg, color-mix(in srgb, var(--vp-c-bg-elv) 94%, transparent), color-mix(in srgb, var(--vp-c-bg-soft) 46%, transparent));
}

.reference-group-header {
  display: grid;
  gap: 0.45rem;
}

.reference-group-header h2 {
  margin: 0;
  border: 0;
  padding: 0;
  font-size: 1.15rem;
}

.reference-group-header h2::after {
  display: none;
}

.reference-group-header p,
.reference-citation,
.reference-note {
  margin: 0;
  color: var(--vp-c-text-2);
  line-height: 1.7;
}

.reference-list {
  display: grid;
  gap: 0.95rem;
  margin: 0;
  padding-left: 1.35rem;
}

.reference-item {
  display: grid;
  gap: 0.35rem;
}

.reference-title-row {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 0.45rem;
}

.reference-link {
  color: var(--vp-c-text-1);
  font-weight: 700;
  text-decoration: none;
}

.reference-link:hover {
  color: var(--vp-c-brand-1);
}

.reference-note {
  font-size: 0.95rem;
}
</style>
