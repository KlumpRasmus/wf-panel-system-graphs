memory_stats_test = executable(
  'memory-stats-test',
  files('memory_stats_test.cpp', '../src/memory_stats.cpp'),
  include_directories: include_directories('../src'),
)

test('memory-stats', memory_stats_test)

system_stats_test = executable(
  'system-stats-test',
  files(
    'system_stats_test.cpp',
    '../src/cpu_sampler.cpp',
    '../src/gpu_sampler.cpp',
  ),
  include_directories: include_directories('../src'),
)

test('system-stats', system_stats_test)

colour_config_test = executable(
  'colour-config-test',
  files('colour_config_test.cpp', '../src/colour_config.cpp'),
  include_directories: include_directories('../src'),
)

test('colour-config', colour_config_test)
