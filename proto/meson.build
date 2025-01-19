wayland_scanner_dep = dependency('wayland-scanner', native: true)
wayland_scanner = find_program(
	wayland_scanner_dep.get_variable('wayland_scanner'),
	native: true,
)

wayland_scanner_server = generator(
	wayland_scanner,
	output: '@BASENAME@-protocol.h',
	arguments: ['server-header', '@INPUT@', '@OUTPUT@'],
)

wayland_scanner_code = generator(
	wayland_scanner,
	output: '@BASENAME@-protocol.c',
	arguments: ['private-code', '@INPUT@', '@OUTPUT@'],
)

wayland_scanner_client = generator(
	wayland_scanner,
	output: '@BASENAME@-client-protocol.h',
	arguments: ['client-header', '@INPUT@', '@OUTPUT@'],
)


protocols = [
    'security-context-v1.xml',
]


wp_security_context_v1_src = []
wp_security_context_v1_headers = []

foreach p : protocols
	xml = join_paths(p)
	wp_security_context_v1_src += wayland_scanner_code.process(xml)
	wp_security_context_v1_headers += wayland_scanner_server.process(xml)
endforeach


lib_wp_security_context_v1 = static_library('security_context_v1', wp_security_context_v1_src + wp_security_context_v1_headers,
	dependencies: [wayland_server]) # for the include directory

wp_security_context_v1 = declare_dependency(
	link_with: lib_wp_security_context_v1,
	sources: wp_security_context_v1_headers,
)