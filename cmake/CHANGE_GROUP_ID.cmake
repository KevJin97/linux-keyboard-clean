add_custom_command(TARGET keyboard-lock POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E echo "Setting group permissions on executable..."
	COMMAND ${CMAKE_SOURCE_DIR}/scripts/set_group_id $<TARGET_FILE:keyboard-lock>
)