#!/usr/bin/env node
/**
 * Sync CHANGELOG.md to docs/release-notes/changelog.md and docs/en/release-notes/changelog.md
 *
 * This script copies the content from the root CHANGELOG.md to the docs site,
 * with formatting changes for both Chinese and English versions.
 *
 * Run from the docs directory: node scripts/sync-changelog.mjs
 */

import { readFileSync, writeFileSync, mkdirSync, existsSync } from "fs";
import { dirname, join } from "path";
import { fileURLToPath } from "url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const docsDir = join(__dirname, "..");
const rootDir = join(docsDir, "..");

const sourcePath = join(rootDir, "CHANGELOG.md");
const zhTargetPath = join(docsDir, "zh/release-notes/changelog.md");
const enTargetPath = join(docsDir, "en/release-notes/changelog.md");

// Headers for each language
const ZH_HEADER = `# 变更记录

本页面记录 FastQTools 每个版本的变更。

`;
const EN_HEADER = `# Changelog

This page documents the changes in each FastQTools release.

`;

// Read the source file
let content = readFileSync(sourcePath, "utf-8");

// Remove the HTML comment block at the top (if any)
content = content.replace(/<!--[\s\S]*?-->\n*/g, "");

// Remove the "# Changelog" title (we'll add our own header)
content = content.replace(/^# Changelog\n+/, "");

// Convert title format: ## [3.1.0] - 2026-04-16 -> ## 3.1.0 (2026-04-16)
content = content.replace(
  /^## \[([^\]]+)\] - (\d{4}-\d{1,2}-\d{1,2})/gm,
  "## $1 ($2)"
);

// Remove the "Change Log Index" section at the bottom (contains dead links)
content = content.replace(/## Change Log Index[\s\S]*$/gm, "");

// Remove subsection headers like ### Added, ### Changed, ### Fixed
// Keep the content but remove the headers for cleaner display
// content = content.replace(/^### (Added|Changed|Fixed|Improved|Tools|SDK)\n+/gm, "");

// Ensure target directories exist
const zhDir = dirname(zhTargetPath);
const enDir = dirname(enTargetPath);

if (!existsSync(zhDir)) {
  mkdirSync(zhDir, { recursive: true });
}
if (!existsSync(enDir)) {
  mkdirSync(enDir, { recursive: true });
}

// Write the Chinese version
writeFileSync(zhTargetPath, ZH_HEADER + content.trim() + "\n");
console.log(`Synced changelog to ${zhTargetPath}`);

// Write the English version (same content, different header)
writeFileSync(enTargetPath, EN_HEADER + content.trim() + "\n");
console.log(`Synced changelog to ${enTargetPath}`);
