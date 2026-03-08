module.exports = {
    extends: ['@commitlint/config-conventional'],
    rules: {
        // 允许的 scope 与项目模块对齐（留空 = 不强制）
        'scope-enum': [1, 'always', [
            'cli', 'io', 'processing', 'stats', 'config', 'error', 'common',
            'benchmark', 'fuzz', 'docker', 'ci', 'deps', 'docs',
        ]],
        'scope-empty': [0],
    },
};
