<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, getSharedLinkLabel, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type Track = {
  title: string
  summary: string
  steps: SharedLinkId[]
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'Reading tracks',
    title: 'Choose the next route by review intent, not by guessing section names.',
    summary:
      'Different readers arrive with different jobs to do. The homepage now turns those jobs into explicit tracks that mirror the information architecture from Task 1.',
    tracks: [
      {
        title: 'Adoption review',
        summary: 'For stakeholders deciding whether FastQTools fits a pipeline, team, or procurement narrative.',
        steps: ['orientationNav', 'whitepaperNav', 'performance'],
      },
      {
        title: 'Implementation onboarding',
        summary: 'For engineers moving from narrative context into code-facing workflows and reference material.',
        steps: ['architecture', 'workflows', 'guideOverview'],
      },
      {
        title: 'Research and maintenance',
        summary: 'For readers validating evidence, contribution expectations, and long-tail support context.',
        steps: ['researchNav', 'resourcesOverview', 'contributing'],
      },
    ] satisfies Track[],
  },
  zh: {
    eyebrow: '阅读路径',
    title: '按评审意图选路线，而不是靠猜章节名。',
    summary:
      '不同读者来到首页时承担的任务并不一样。新版首页把这些任务翻译成明确路径，并与任务 1 产出的信息架构保持镜像。',
    tracks: [
      {
        title: '采用评审',
        summary: '适合判断 FastQTools 是否匹配当前流程、团队或采购叙事的读者。',
        steps: ['orientationNav', 'whitepaperNav', 'performance'],
      },
      {
        title: '实施入门',
        summary: '适合需要从叙事背景走向代码工作流与参考资料的工程师。',
        steps: ['architecture', 'workflows', 'guideOverview'],
      },
      {
        title: '研究与维护',
        summary: '适合核验证据、贡献约束与长期维护语境的读者。',
        steps: ['researchNav', 'resourcesOverview', 'contributing'],
      },
    ] satisfies Track[],
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

    <div class="track-grid">
      <article v-for="track in content.tracks" :key="track.title" class="paper-card track-card">
        <h3>{{ track.title }}</h3>
        <p>{{ track.summary }}</p>
        <ol class="track-steps">
          <li v-for="step in track.steps" :key="step">
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
