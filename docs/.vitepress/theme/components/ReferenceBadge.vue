<script setup lang="ts">
import { computed } from 'vue'

const props = withDefaults(defineProps<{
  kind?: string
  href?: string
  title?: string
}>(), {
  kind: 'Reference',
})

const normalizedKind = computed(() => props.kind.toLowerCase().replace(/[^a-z0-9]+/g, '-'))
const isExternal = computed(() => Boolean(props.href && /^https?:\/\//.test(props.href)))
</script>

<template>
  <component
    :is="href ? 'a' : 'span'"
    class="reference-badge"
    :class="`tone-${normalizedKind}`"
    :href="href"
    :title="title"
    :target="isExternal ? '_blank' : undefined"
    :rel="isExternal ? 'noreferrer' : undefined"
  >
    <slot>{{ kind }}</slot>
  </component>
</template>

<style scoped>
.reference-badge {
  display: inline-flex;
  align-items: center;
  gap: 0.25rem;
  margin: 0 0.16rem;
  padding: 0.14rem 0.52rem;
  border: 1px solid color-mix(in srgb, var(--vp-c-border) 82%, var(--vp-c-brand-1));
  border-radius: 999px;
  background: color-mix(in srgb, var(--vp-c-bg-soft) 86%, transparent);
  color: var(--vp-c-text-1);
  font-size: 0.78rem;
  font-weight: 700;
  line-height: 1.3;
  vertical-align: baseline;
  text-decoration: none;
  white-space: nowrap;
}

.reference-badge:hover {
  border-color: var(--vp-c-brand-1);
  color: var(--vp-c-brand-1);
}

.tone-rfc,
.tone-report {
  background: color-mix(in srgb, var(--vp-c-brand-soft) 82%, var(--vp-c-bg-soft));
}

.tone-spec,
.tone-standard {
  background: color-mix(in srgb, var(--vp-c-green-soft) 84%, var(--vp-c-bg-soft));
  border-color: color-mix(in srgb, var(--vp-c-green-2) 38%, var(--vp-c-border));
}

.tone-paper,
.tone-library {
  background: color-mix(in srgb, var(--vp-c-yellow-soft) 84%, var(--vp-c-bg-soft));
  border-color: color-mix(in srgb, var(--vp-c-yellow-2) 36%, var(--vp-c-border));
}

.tone-project,
.tone-tool {
  background: color-mix(in srgb, var(--vp-c-indigo-soft) 84%, var(--vp-c-bg-soft));
  border-color: color-mix(in srgb, var(--vp-c-indigo-2) 34%, var(--vp-c-border));
}
</style>
