<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight, BookOpen, Languages, ShieldCheck } from 'lucide-vue-next'
import { getSharedLinkHref, homepageActionLinkIds, type LocaleKey } from '../content/siteNavigation'

type ActionLinkId = (typeof homepageActionLinkIds)[keyof typeof homepageActionLinkIds]

type Action = {
  label: string
  linkId: ActionLinkId
  variant: 'primary' | 'secondary'
}

type Signal = {
  title: string
  detail: string
  icon: typeof BookOpen
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'FastQTools docs',
    title: 'A bilingual technical whitepaper hub for FASTQ evaluation and adoption.',
    summary: 'Start with the story, validate the evidence, then drop into commands, APIs, and developer entry points.',
    actions: [
      { label: 'Why FastQTools', linkId: homepageActionLinkIds.primary, variant: 'primary' },
      { label: 'Get started', linkId: homepageActionLinkIds.secondary, variant: 'secondary' },
    ] satisfies Action[],
    signals: [
      {
        title: 'Two focused workflows',
        detail: 'The maintained product surface stays intentionally narrow: stat for QC reports and filter for single-pass preprocessing.',
        icon: BookOpen,
      },
      {
        title: 'Spec-driven maintenance',
        detail: 'Current behavior is anchored in openspec baseline documents so evaluators can trace decisions back to maintained source material.',
        icon: ShieldCheck,
      },
      {
        title: 'Built for bilingual reading',
        detail: 'Chinese and English narrative paths stay aligned so teams can review the same evidence in either language.',
        icon: Languages,
      },
    ] satisfies Signal[],
  },
  zh: {
    eyebrow: 'FastQTools 文档',
    title: '面向 FASTQ 评估与采用决策的双语技术白皮书入口。',
    summary: '先理解项目定位，再核对性能与工作流证据，最后进入命令、API 与开发者入口。',
    actions: [
      { label: '为什么选择 FastQTools', linkId: homepageActionLinkIds.primary, variant: 'primary' },
      { label: '快速开始', linkId: homepageActionLinkIds.secondary, variant: 'secondary' },
    ] satisfies Action[],
    signals: [
      {
        title: '聚焦两个核心工作流',
        detail: '当前维护的产品面保持克制：stat 用于质控报告，filter 用于单次过滤与预处理。',
        icon: BookOpen,
      },
      {
        title: '规范驱动维护',
        detail: '当前行为以 openspec baseline 为准，方便评估者追溯决策依据与维护边界。',
        icon: ShieldCheck,
      },
      {
        title: '双语阅读路径同步',
        detail: '中文与英文首页保持一致的信息架构，便于团队在不同语言环境下共用同一组证据。',
        icon: Languages,
      },
    ] satisfies Signal[],
  },
} as const

const content = computed(() => copy[props.locale])
const resolve = (linkId: ActionLinkId) => withBase(getSharedLinkHref(props.locale, linkId))
</script>

<template>
  <section class="hero-panel">
    <div class="hero-copy">
      <p class="eyebrow">{{ content.eyebrow }}</p>
      <h1>{{ content.title }}</h1>
      <p class="summary">{{ content.summary }}</p>
      <div class="actions">
        <a
          v-for="action in content.actions"
          :key="action.linkId"
          :class="['action', action.variant]"
          :href="resolve(action.linkId)"
        >
          <span>{{ action.label }}</span>
          <ArrowRight :size="16" />
        </a>
      </div>
    </div>

    <div class="signal-grid">
      <article v-for="signal in content.signals" :key="signal.title" class="signal-card">
        <component :is="signal.icon" :size="20" class="signal-icon" />
        <h2>{{ signal.title }}</h2>
        <p>{{ signal.detail }}</p>
      </article>
    </div>
  </section>
</template>
