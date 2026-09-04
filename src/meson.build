gtkmm = dependency('gtkmm-3.0', version: '>=3.24')

# Raspberry Pi OS currently installs a wf-panel-pi.pc file whose Version field
# may be "undefined", so Meson cannot apply a numeric version constraint here.
wfpanel = dependency('wf-panel-pi')

common_sources = files('usage_graph.cpp')
common_dependencies = [gtkmm, wfpanel]

shared_module(
  'libmemory',
  common_sources + files('memory_plugin.cpp', 'memory_stats.cpp'),
  dependencies: common_dependencies,
  install: true,
  install_dir: get_option('libdir') / 'wf-panel-pi',
  cpp_args: ['-DGETTEXT_PACKAGE="wfplug_memory"'],
  name_prefix: '',
)

shared_module(
  'libcpugraph',
  common_sources + files('cpu_plugin.cpp', 'cpu_sampler.cpp'),
  dependencies: common_dependencies,
  install: true,
  install_dir: get_option('libdir') / 'wf-panel-pi',
  cpp_args: ['-DGETTEXT_PACKAGE="wfplug_cpugraph"'],
  name_prefix: '',
)

shared_module(
  'libgpugraph',
  common_sources + files('gpu_plugin.cpp', 'gpu_sampler.cpp'),
  dependencies: common_dependencies,
  install: true,
  install_dir: get_option('libdir') / 'wf-panel-pi',
  cpp_args: ['-DGETTEXT_PACKAGE="wfplug_gpugraph"'],
  name_prefix: '',
)

install_data(
  'memory.xml',
  'cpugraph.xml',
  'gpugraph.xml',
  install_dir: metadata_dir,
)
