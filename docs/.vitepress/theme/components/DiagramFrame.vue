<script setup lang="ts">
import { computed, useSlots } from 'vue'

import architectureOverviewSvg from '../../../assets/diagrams/architecture-overview.svg?raw'
import executionModelSvg from '../../../assets/diagrams/execution-model.svg?raw'
import readingMapSvg from '../../../assets/diagrams/reading-map.svg?raw'

const props = defineProps<{
  caption?: string
  asset?: 'architecture-overview' | 'execution-model' | 'reading-map'
}>()
const slots = useSlots()

const diagramAssets = {
  'architecture-overview': architectureOverviewSvg,
  'execution-model': executionModelSvg,
  'reading-map': readingMapSvg,
} as const

const diagramMarkup = computed(() => (props.asset ? diagramAssets[props.asset] : null))
const hasCaptionSlot = computed(() => Boolean(slots.caption))
</script>

<template>
  <figure class="diagram-frame">
    <div v-if="diagramMarkup" class="diagram-frame__asset" v-html="diagramMarkup" />
    <slot v-else />
    <figcaption v-if="caption || hasCaptionSlot">
      <slot name="caption">{{ props.caption }}</slot>
    </figcaption>
  </figure>
</template>
