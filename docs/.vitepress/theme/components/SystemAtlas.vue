<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type LedgerCard = {
  title: string
  detail: string
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'System atlas',
    title: 'See the maintained execution and evidence topology in one frame.',
    summary:
      'The atlas keeps product boundary, execution stages, and reference surfaces in the same diagram so reviewers can move from narrative claims to exact chapters without losing the systems model.',
    cards: [
      {
        title: 'Source discipline',
        detail: 'Plain and gzip FASTQ enter through one bounded ingestion path before any QC operator is applied.',
      },
      {
        title: 'Processing contract',
        detail: 'Filtering, trimming, and statistics stay inside the staged pipeline where concurrency and ownership remain explicit.',
      },
      {
        title: 'Evidence closure',
        detail: 'Benchmark, RFC, and reference surfaces are modeled as first-class outputs of the same system.',
      },
    ] satisfies LedgerCard[],
    actions: [
      { label: 'Architecture chapter', linkId: 'architecture' },
      { label: 'Algorithm chapter', linkId: 'algorithms' },
      { label: 'Performance chapter', linkId: 'performance' },
    ] satisfies { label: string; linkId: SharedLinkId }[],
  },
  zh: {
    eyebrow: '系统图谱',
    title: '在一张图里同时看到维护中的执行拓扑与证据拓扑。',
    summary:
      '这张 atlas 把产品边界、执行阶段与参考表面放进同一个视图里，让评审者从首页叙事切换到精确章节时不会丢失系统心智模型。',
    cards: [
      {
        title: 'Source 纪律',
        detail: '普通与 gzip FASTQ 先经过统一且有界的摄取路径，之后才进入任何 QC 运算。',
      },
      {
        title: 'Processing 契约',
        detail: '过滤、修剪与统计始终留在分阶段流水线里，并发与所有权边界保持显式。',
      },
      {
        title: '证据闭环',
        detail: 'benchmark、RFC 与 reference 路线被建模成同一系统的正式输出，而不是附录尾注。',
      },
    ] satisfies LedgerCard[],
    actions: [
      { label: '架构章节', linkId: 'architecture' },
      { label: '算法章节', linkId: 'algorithms' },
      { label: '性能章节', linkId: 'performance' },
    ] satisfies { label: string; linkId: SharedLinkId }[],
  },
} as const

const content = computed(() => copy[props.locale])
const resolve = (linkId: SharedLinkId) => withBase(getSharedLinkHref(props.locale, linkId))
</script>

<template>
  <section class="home-whitepaper home-section system-atlas">
    <div class="section-header">
      <p class="section-kicker">{{ content.eyebrow }}</p>
      <h2 class="section-heading">{{ content.title }}</h2>
      <p class="section-lead">{{ content.summary }}</p>
    </div>

    <div class="system-atlas-grid">
      <figure class="diagram-frame system-atlas-figure">
        <svg
          viewBox="0 0 960 520"
          class="publication-diagram"
          role="img"
          aria-labelledby="system-atlas-title system-atlas-desc"
        >
          <title id="system-atlas-title">
            {{ props.locale === 'en' ? 'FastQTools system atlas' : 'FastQTools 系统图谱' }}
          </title>
          <desc id="system-atlas-desc">
            {{
              props.locale === 'en'
                ? 'Bounded ingest, processing, and evidence surfaces rendered with shared theme tokens.'
                : '使用共享主题 token 绘制的有界摄取、处理与证据表面。'
            }}
          </desc>

          <rect x="24" y="24" width="912" height="472" rx="28" fill="var(--fq-diagram-bg)" />
          <circle cx="480" cy="246" r="188" fill="var(--fq-diagram-glow)" />
          <g class="diagram-grid" stroke="var(--fq-diagram-grid)" stroke-width="1">
            <path d="M180 54 V468" />
            <path d="M480 54 V468" />
            <path d="M780 54 V468" />
            <path d="M54 214 H906" />
            <path d="M54 352 H906" />
          </g>
          <rect
            x="56"
            y="72"
            width="248"
            height="280"
            rx="24"
            fill="var(--fq-diagram-layer)"
            stroke="var(--fq-diagram-stroke)"
            stroke-width="2"
          />
          <rect
            x="356"
            y="72"
            width="248"
            height="280"
            rx="24"
            fill="var(--fq-diagram-layer)"
            stroke="var(--fq-diagram-accent)"
            stroke-width="2.5"
          />
          <rect
            x="656"
            y="72"
            width="248"
            height="280"
            rx="24"
            fill="var(--fq-diagram-layer)"
            stroke="var(--fq-diagram-signal)"
            stroke-width="2.5"
          />
          <rect
            x="88"
            y="118"
            width="184"
            height="56"
            rx="18"
            fill="var(--fq-diagram-bg)"
            stroke="var(--fq-diagram-stroke)"
            stroke-width="2"
          />
          <rect
            x="388"
            y="118"
            width="184"
            height="56"
            rx="18"
            fill="var(--fq-diagram-bg)"
            stroke="var(--fq-diagram-accent)"
            stroke-width="2"
          />
          <rect
            x="688"
            y="118"
            width="184"
            height="56"
            rx="18"
            fill="var(--fq-diagram-bg)"
            stroke="var(--fq-diagram-signal)"
            stroke-width="2"
          />

          <path
            d="M304 214 C334 214 334 214 356 214"
            fill="none"
            stroke="var(--fq-diagram-stroke)"
            stroke-width="4"
            stroke-linecap="round"
          />
          <path
            d="M604 214 C634 214 634 214 656 214"
            fill="none"
            stroke="var(--fq-diagram-accent)"
            stroke-width="4"
            stroke-linecap="round"
          />
          <path
            d="M480 352 L480 430"
            fill="none"
            stroke="var(--fq-diagram-stroke)"
            stroke-width="4"
            stroke-linecap="round"
          />

          <rect
            x="300"
            y="402"
            width="360"
            height="58"
            rx="20"
            fill="var(--fq-diagram-bg)"
            stroke="var(--fq-diagram-stroke)"
            stroke-width="2"
          />

          <text x="80" y="106" fill="var(--fq-diagram-muted)" font-size="16" font-weight="700">
            <tspan x="80" dy="0">{{ props.locale === 'en' ? 'SOURCE' : 'SOURCE' }}</tspan>
          </text>
          <text x="380" y="106" fill="var(--fq-diagram-muted)" font-size="16" font-weight="700">
            <tspan x="380" dy="0">{{ props.locale === 'en' ? 'PROCESSING' : 'PROCESSING' }}</tspan>
          </text>
          <text x="680" y="106" fill="var(--fq-diagram-muted)" font-size="16" font-weight="700">
            <tspan x="680" dy="0">{{ props.locale === 'en' ? 'EVIDENCE' : 'EVIDENCE' }}</tspan>
          </text>

          <text x="110" y="152" fill="var(--fq-diagram-text)" font-size="20" font-weight="700">
            <tspan x="110" dy="0">{{ props.locale === 'en' ? 'FASTQ ingest' : 'FASTQ 摄取' }}</tspan>
          </text>
          <text x="110" y="206" fill="var(--fq-diagram-text)" font-size="18">
            <tspan x="110" dy="0">{{ props.locale === 'en' ? 'gzip aware readers' : 'gzip 感知 reader' }}</tspan>
            <tspan x="110" dy="24">{{ props.locale === 'en' ? 'bounded batch formation' : '有界批次形成' }}</tspan>
            <tspan x="110" dy="24">{{ props.locale === 'en' ? 'string_view-safe records' : 'string_view 安全记录' }}</tspan>
          </text>

          <text x="410" y="152" fill="var(--fq-diagram-text)" font-size="20" font-weight="700">
            <tspan x="410" dy="0">{{ props.locale === 'en' ? 'QC pipeline' : 'QC 流水线' }}</tspan>
          </text>
          <text x="410" y="206" fill="var(--fq-diagram-text)" font-size="18">
            <tspan x="410" dy="0">{{ props.locale === 'en' ? 'predicates + mutators' : 'predicate + mutator' }}</tspan>
            <tspan x="410" dy="24">{{ props.locale === 'en' ? 'statistics stages' : '统计阶段' }}</tspan>
            <tspan x="410" dy="24">{{ props.locale === 'en' ? 'oneTBB orchestration' : 'oneTBB 编排' }}</tspan>
          </text>

          <text x="710" y="152" fill="var(--fq-diagram-text)" font-size="20" font-weight="700">
            <tspan x="710" dy="0">{{ props.locale === 'en' ? 'Review outputs' : '评审输出' }}</tspan>
          </text>
          <text x="710" y="206" fill="var(--fq-diagram-text)" font-size="18">
            <tspan x="710" dy="0">{{ props.locale === 'en' ? 'benchmark report' : 'benchmark 报告' }}</tspan>
            <tspan x="710" dy="24">{{ props.locale === 'en' ? 'RFC and specs' : 'RFC 与规范' }}</tspan>
            <tspan x="710" dy="24">{{ props.locale === 'en' ? 'reference chapters' : 'reference 章节' }}</tspan>
          </text>

          <text x="342" y="438" fill="var(--fq-diagram-text)" font-size="18" font-weight="600">
            <tspan x="342" dy="0">
              {{
                props.locale === 'en'
                  ? 'Control surfaces stay aligned from homepage narrative to implementation detail.'
                  : '控制界面从首页叙事一直保持到实现细节。'
              }}
            </tspan>
          </text>
        </svg>
      </figure>

      <div class="system-atlas-ledger">
        <article v-for="card in content.cards" :key="card.title" class="paper-card atlas-card">
          <h3>{{ card.title }}</h3>
          <p>{{ card.detail }}</p>
        </article>

        <div class="section-actions compact system-atlas-actions">
          <a v-for="action in content.actions" :key="action.linkId" class="paper-link tertiary" :href="resolve(action.linkId)">
            <span>{{ action.label }}</span>
            <ArrowRight :size="16" />
          </a>
        </div>
      </div>
    </div>
  </section>
</template>
