project(
  'system-graphs',
  'cpp',
  version: '0.5.0',
  meson_version: '>=0.60.0',
  default_options: [
    'cpp_std=c++17',
    'warning_level=2',
    'werror=false',
  ],
)

share_dir = get_option('prefix') / 'share'
wfpanel_data_dir = share_dir / 'wf-panel-pi'
metadata_dir = wfpanel_data_dir / 'metadata'

add_project_arguments(
  '-D_GNU_SOURCE',
  language: 'cpp',
)

subdir('src')
subdir('tests')
