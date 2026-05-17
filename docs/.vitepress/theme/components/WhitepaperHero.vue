<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight, FileStack, Gauge, Network } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type Action = {
  label: string
  linkId: SharedLinkId
  variant: 'primary' | 'secondary'
}

type Signal = {
  title: string
  detail: string
  icon: typeof FileStack
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'FastQTools technical whitepaper',
    title: 'A research-grade FASTQ systems brief for adoption review and engineering transfer.',
    summary:
      'The homepage now behaves like the front page of a whitepaper: it frames system scope, maps the execution model, isolates algorithm pillars, and points reviewers toward evidence and follow-up chapters without forcing them through a generic card wall.',
    actions: [
      { label: 'Start with orientation', linkId: 'orientationNav', variant: 'primary' },
      { label: 'Enter the whitepaper', linkId: 'whitepaperNav', variant: 'secondary' },
      { label: 'Inspect performance evidence', linkId: 'performance', variant: 'secondary' },
    ] satisfies Action[],
    signals: [
      {
        title: 'Architecture-first narrative',
        detail: 'The hero treats adoption review, algorithm study, and implementation handoff as one technical story.',
        icon: FileStack,
      },
      {
        title: 'Execution model in context',
        detail: 'zero-copy I/O, oneTBB orchestration, and QC operators are surfaced as system-level decisions rather than isolated feature bullets.',
        icon: Network,
      },
      {
        title: 'Evidence kept adjacent',
        detail: 'benchmark, governance, and reference routes stay visible wherever a reviewer needs to validate a claim.',
        icon: Gauge,
      },
    ] satisfies Signal[],
  },
  zh: {
    eyebrow: 'FastQTools 技术白皮书',
    title: '面向采用评审与工程移交的 FASTQ 系统级研究首页。',
    summary:
      '首页不再是泛化卡片堆，而是白皮书首页：先界定系统范围，再给出执行模型与算法支柱，并把证据、章节入口和后续阅读路径组织成可供评审直接引用的技术结构。',
    actions: [
      { label: '从导读开始', linkId: 'orientationNav', variant: 'primary' },
      { label: '进入白皮书', linkId: 'whitepaperNav', variant: 'secondary' },
      { label: '查看性能证据', linkId: 'performance', variant: 'secondary' },
    ] satisfies Action[],
    signals: [
      {
        title: '以架构为主线的叙事',
        detail: '采用判断、算法理解和实施移交被放进同一条技术叙事，而不是拆散成零散卖点。',
        icon: FileStack,
      },
      {
        title: '执行模型直接入场',
        detail: '零拷贝 I/O、oneTBB 流水线与 QC 运算被当作系统决策说明，而不是孤立功能点。',
        icon: Network,
      },
      {
        title: '证据始终贴身',
        detail: '性能、治理与参考入口被保留在视野内，方便评审在任何阶段立即追溯依据。',
        icon: Gauge,
      },
    ] satisfies Signal[],
  },
} as const

const content = computed(() => copy[props.locale])
const resolve = (linkId: SharedLinkId) => withBase(getSharedLinkHref(props.locale, linkId))
</script>

<template>
  <section class="home-whitepaper hero-whitepaper">
    <div class="home-hero-grid">
      <div class="hero-copy">
        <p class="section-kicker">{{ content.eyebrow }}</p>
        <h1 class="hero-title">{{ content.title }}</h1>
        <p class="section-lead hero-summary">{{ content.summary }}</p>

        <div class="section-actions">
          <a
            v-for="action in content.actions"
            :key="action.linkId"
            :class="['paper-link', action.variant]"
            :href="resolve(action.linkId)"
          >
            <span>{{ action.label }}</span>
            <ArrowRight :size="16" />
          </a>
        </div>
      </div>

      <aside class="paper-card hero-brief">
        <p class="section-kicker">{{ props.locale === 'en' ? 'Executive frame' : '执行摘要框架' }}</p>
        <h2 class="section-heading brief-heading">
          {{ props.locale === 'en' ? 'Why this landing page now reads like a systems paper' : '为什么首页现在像一篇系统白皮书' }}
        </h2>
        <div class="paper-metric-grid">
          <article v-for="signal in content.signals" :key="signal.title" class="paper-metric">
            <div class="metric-icon">
              <component :is="signal.icon" :size="18" />
            </div>
            <div>
              <h3>{{ signal.title }}</h3>
              <p>{{ signal.detail }}</p>
            </div>
          </article>
        </div>
      </aside>
    </div>
  </section>
</template>
