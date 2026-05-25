<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight, BookOpen, Languages, ShieldCheck } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type Action = {
  label: string
  linkId: SharedLinkId
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
    eyebrow: 'FastQTools technical whitepaper',
    title: 'Choose the reading path that fits your FASTQ evaluation stage.',
    summary: 'Start on the maintained whitepaper surface, then continue into algorithms guidance or research evidence when you need deeper validation.',
    actions: [
      { label: 'Read the whitepaper', linkId: 'whitepaper', variant: 'primary' },
      { label: 'Open reference', linkId: 'referenceNav', variant: 'secondary' },
    ] satisfies Action[],
    signals: [
      {
        title: 'whitepaper for first-pass triage',
        detail: 'Use the whitepaper when you need the fastest maintained explanation of section boundaries, audience fit, and where to read next.',
        icon: BookOpen,
      },
      {
        title: 'technical whitepaper for decision reviews',
        detail: 'Positioning, architectural rationale, and benchmark claims stay together so adoption reviews can cite one coherent narrative.',
        icon: ShieldCheck,
      },
      {
        title: 'research-ready follow-up',
        detail: 'Performance evidence, resources, and maintenance context remain close by whenever research, procurement, or implementation planning needs more proof.',
        icon: Languages,
      },
    ] satisfies Signal[],
  },
  zh: {
    eyebrow: 'FastQTools 技术白皮书',
    title: '为 FASTQ 采用评估选择合适的阅读路径。',
    summary: '先从当前维护中的白皮书入口开始；当你需要更深的落地说明时，再转向算法层或研究证据。',
    actions: [
      { label: '阅读白皮书', linkId: 'whitepaper', variant: 'primary' },
      { label: '打开参考导航', linkId: 'referenceNav', variant: 'secondary' },
    ] satisfies Action[],
    signals: [
      {
        title: '白皮书负责第一次分流',
        detail: '当你需要最快理解当前维护中的分区边界、适合对象与下一步阅读建议时，先看白皮书最省时间。',
        icon: BookOpen,
      },
      {
        title: '技术白皮书负责做出判断',
        detail: '项目定位、架构理由与 benchmark 主张被放在同一条叙事里，方便用于采用评审与技术答辩。',
        icon: ShieldCheck,
      },
      {
        title: '研究层负责补齐证据',
        detail: '当研究、采购或实施规划需要更多依据时，性能证据、资源入口与维护背景都可以继续向下追踪。',
        icon: Languages,
      },
    ] satisfies Signal[],
  },
} as const

const content = computed(() => copy[props.locale])
const resolve = (linkId: SharedLinkId) => withBase(getSharedLinkHref(props.locale, linkId))
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
