<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type EvidenceCard = {
  label: string
  value: string
  detail: string
  linkId: SharedLinkId
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'Evidence deck',
    title: 'Treat metrics, contracts, and references as one review packet.',
    summary:
      'A technical whitepaper is only useful when the reader can jump from a claim to its supporting chapter immediately. The evidence deck keeps those supporting chapters adjacent to the homepage argument.',
    cards: [
      {
        label: 'Throughput class',
        value: 'Up to 1.7M reads/s',
        detail: 'Performance pages keep the representative benchmark envelope and the caveats in the same narrative.',
        linkId: 'performance',
      },
      {
        label: 'Execution discipline',
        value: 'Zero-copy batches',
        detail: 'Architecture and algorithms chapters explain why string_view lifetime and pool reuse are design issues.',
        linkId: 'algorithms',
      },
      {
        label: 'Research support',
        value: 'Bibliography + RFCs',
        detail: 'Formal references, related-project context, and evolution notes remain part of the maintained story.',
        linkId: 'bibliography',
      },
    ] satisfies EvidenceCard[],
  },
  zh: {
    eyebrow: '证据卡组',
    title: '把指标、契约与引用组织成同一份评审材料。',
    summary:
      '技术白皮书只有在读者能立刻从主张跳到支撑章节时才真正有用。证据卡组的作用，就是把这些支撑路径固定在首页论证旁边。',
    cards: [
      {
        label: '吞吐等级',
        value: '最高约 1.7M reads/s',
        detail: '性能页把代表性 benchmark 包络与所有条件说明放在同一条叙事里。',
        linkId: 'performance',
      },
      {
        label: '执行纪律',
        value: '零拷贝批处理',
        detail: '架构与算法章节会解释为什么 string_view 生命周期与池化复用属于设计问题。',
        linkId: 'algorithms',
      },
      {
        label: '研究支撑',
        value: '书目 + RFC',
        detail: '正式参考、相关项目语境与演进说明始终保留在维护中的叙事里。',
        linkId: 'bibliography',
      },
    ] satisfies EvidenceCard[],
  },
} as const

const content = computed(() => copy[props.locale])
const resolve = (linkId: SharedLinkId) => withBase(getSharedLinkHref(props.locale, linkId))
</script>

<template>
  <section class="home-whitepaper home-section">
    <div class="section-header">
      <p class="section-kicker">{{ content.eyebrow }}</p>
      <h2 class="section-heading">{{ content.title }}</h2>
      <p class="section-lead">{{ content.summary }}</p>
    </div>

    <div class="evidence-grid">
      <article v-for="card in content.cards" :key="card.label" class="paper-card evidence-card">
        <p class="evidence-label">{{ card.label }}</p>
        <h3 class="evidence-value">{{ card.value }}</h3>
        <p>{{ card.detail }}</p>
        <a class="paper-link tertiary evidence-link" :href="resolve(card.linkId)">
          <span>{{ props.locale === 'en' ? 'Read supporting chapter' : '阅读支撑章节' }}</span>
          <ArrowRight :size="16" />
        </a>
      </article>
    </div>
  </section>
</template>
