<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, getSharedLinkLabel, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type Lane = {
  title: string
  summary: string
  steps: SharedLinkId[]
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'Reading lanes',
    title: 'Pick the next chapter by review intent, not by guessing the site map.',
    summary:
      'The homepage closes the navigation gap for senior readers: each lane translates a job-to-be-done into a short sequence of maintained chapters.',
    lanes: [
      {
        title: 'Adoption diligence',
        summary: 'Use this route when the question is whether FastQTools belongs in a team, platform, or procurement story.',
        steps: ['orientationNav', 'whitepaperNav', 'performance'],
      },
      {
        title: 'Implementation handoff',
        summary: 'Use this route when narrative review is complete and engineering detail now matters.',
        steps: ['architecture', 'algorithms', 'referenceNav'],
      },
      {
        title: 'Research context',
        summary: 'Use this route when you need sources, adjacent-project comparison, or historical rationale.',
        steps: ['researchNav', 'bibliography', 'evolutionNotes'],
      },
    ] satisfies Lane[],
  },
  zh: {
    eyebrow: '阅读车道',
    title: '按评审任务选下一章，而不是靠猜网站结构。',
    summary:
      '这一节用于补上高级读者的导航断层：每条 lane 都把一个 job-to-be-done 翻译成一串维护中的章节。',
    lanes: [
      {
        title: '采用尽调',
        summary: '当问题是 FastQTools 是否适合团队、平台或采购叙事时，请走这条路线。',
        steps: ['orientationNav', 'whitepaperNav', 'performance'],
      },
      {
        title: '实施移交',
        summary: '当叙事评审已经完成、工程细节开始变得重要时，请走这条路线。',
        steps: ['architecture', 'algorithms', 'referenceNav'],
      },
      {
        title: '研究语境',
        summary: '当你需要来源、同类项目对照或历史理由时，请走这条路线。',
        steps: ['researchNav', 'bibliography', 'evolutionNotes'],
      },
    ] satisfies Lane[],
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

    <div class="track-grid reading-lanes-grid">
      <article v-for="lane in content.lanes" :key="lane.title" class="paper-card track-card">
        <h3>{{ lane.title }}</h3>
        <p>{{ lane.summary }}</p>
        <ol class="track-steps">
          <li v-for="step in lane.steps" :key="step">
            <a class="track-step-link" :href="resolve(step)">
              <span>{{ getSharedLinkLabel(props.locale, step) }}</span>
              <ArrowRight :size="15" />
            </a>
          </li>
        </ol>
      </article>
    </div>
  </section>
</template>
