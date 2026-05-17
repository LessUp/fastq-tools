<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, getSharedLinkLabel, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

const props = defineProps<{ locale: LocaleKey }>()

const links = ['performance', 'benchmarkReport', 'resourcesOverview'] as const satisfies readonly SharedLinkId[]

const copy = {
  en: {
    eyebrow: 'References stay in context',
    summary: 'References, benchmark method notes, and related-project context appear alongside the narrative so readers can validate claims without leaving the reading path.',
  },
  zh: {
    eyebrow: '引用与证据不再躲在附录',
    summary: '参考文献、基准方法说明与相关项目背景会直接贴着叙事出现，方便读者在阅读路径中随时核对依据。',
  },
} as const

const content = computed(() => copy[props.locale])
const resolve = (id: SharedLinkId) => withBase(getSharedLinkHref(props.locale, id))
</script>

<template>
  <section class="citation-strip">
    <div class="citation-copy">
      <p class="eyebrow">{{ content.eyebrow }}</p>
      <p>{{ content.summary }}</p>
    </div>
    <ul class="citation-links">
      <li v-for="id in links" :key="id">
        <a :href="resolve(id)">
          <span>{{ getSharedLinkLabel(props.locale, id) }}</span>
          <ArrowRight :size="16" />
        </a>
      </li>
    </ul>
  </section>
</template>

<style scoped>
.citation-strip {
  display: grid;
  gap: 1rem;
  margin: 0 0 2rem;
  padding: 1.15rem 1.2rem;
  border: 1px solid var(--vp-c-border);
  border-radius: var(--radius-lg);
  background:
    linear-gradient(135deg, color-mix(in srgb, var(--fq-c-brand-soft) 70%, transparent), transparent 50%),
    color-mix(in srgb, var(--vp-c-bg-elv) 92%, transparent);
}

.citation-copy {
  display: grid;
  gap: 0.55rem;
}

.eyebrow {
  margin: 0;
  color: var(--vp-c-brand-1);
  font-size: 0.8rem;
  font-weight: 700;
  letter-spacing: 0.08em;
  text-transform: uppercase;
}

.citation-copy p:last-child {
  margin: 0;
  color: var(--vp-c-text-2);
}

.citation-links {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(12rem, 1fr));
  gap: 0.75rem;
  margin: 0;
  padding: 0;
  list-style: none;
}

.citation-links a {
  display: inline-flex;
  align-items: center;
  justify-content: space-between;
  gap: 0.55rem;
  min-height: 2.8rem;
  padding: 0.7rem 0.9rem;
  border: 1px solid var(--vp-c-border);
  border-radius: var(--radius-md);
  background: color-mix(in srgb, var(--vp-c-bg-soft) 72%, transparent);
  color: var(--vp-c-text-1);
  font-weight: 600;
  transition: border-color var(--transition-normal), background var(--transition-normal);
}

.citation-links a:hover {
  border-color: var(--fq-c-border-strong);
  background: color-mix(in srgb, var(--fq-c-brand-soft) 80%, var(--vp-c-bg-elv));
}

@media (max-width: 640px) {
  .citation-strip {
    padding: 1rem;
  }

  .citation-links {
    grid-template-columns: 1fr;
  }
}
</style>
