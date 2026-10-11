#if defined _INC_mapping
	#endinput
#endif
#define _INC_mapping

/*
==============================================================================
 mapping.inc (REMOVED)
 Custom world mapping objects have been removed per user instruction to
 maintain clean GTA SA default world geometry and prevent collision issues.
==============================================================================
*/

stock LoadNusantaraMapping()
{
	// Clean GTA San Andreas default world mapping (0 custom collision objects)
	return 1;
}
