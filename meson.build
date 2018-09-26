project('amsel', 'c',        version: '0.1.0',
  meson_version: '>= 0.42.0',
)

i18n = import('i18n')

config_h = configuration_data()
config_h.set_quoted('GETTEXT_PACKAGE', 'amsel')
config_h.set_quoted('LOCALEDIR', join_paths(get_option('prefix'), get_option('localedir')))
config_h.set_quoted('BUILDDIR', meson.build_root())
config_h.set_quoted('SRCDIR', meson.source_root())
config_h.set_quoted('PACKAGE_DATADIR', join_paths(get_option('prefix'), get_option('datadir')))
config_h.set_quoted('PACKAGE_LIBDIR', join_paths(get_option('prefix'), get_option('libdir')))
configure_file(
  output: 'amsel-config.h',
  configuration: config_h,
)
add_project_arguments([
  '-Werror=implicit-function-declaration',
  '-I' + meson.build_root(),
], language: 'c')

aml_prefix = get_option('prefix')
girdir = join_paths(aml_prefix, get_option('datadir'), 'gir-1.0')
typelibdir = join_paths(aml_prefix, get_option('libdir'), 'girepository-1.0')

subdir('data')
subdir('lib')
subdir('src')
subdir('test')
subdir('doc')
subdir('build-aux')