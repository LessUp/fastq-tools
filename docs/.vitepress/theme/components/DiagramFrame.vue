<script setup lang="ts">
defineProps<{
  caption?: string
}>()
</script>

<template>
  <figure class="diagram-frame">
    <slot />
    <figcaption v-if="caption">
      <slot name="caption">{{ caption }}</slot>
    </figcaption>
  </figure>
</template>
