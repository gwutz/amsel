project('alb', 'c',        version: '0.1.0',
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
  '-I' + meson.build_root(),
], language: 'c')

package_subdir = get_option('package_subdir') # When used as subproject
girdir = join_paths(get_option('datadir'), package_subdir, 'gir-1.0')
typelibdir = join_paths(get_option('libdir'), package_subdir, 'girepository-1.0')

subdir('data')
subdir('lib')
subdir('src')
subdir('test')
subdir('doc')
subdir('build-aux')