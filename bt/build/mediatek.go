package fluoride

import (
	"android/soong/android"
	"android/soong/android/mediatek"
)

func mtkGlobalDefaults(ctx android.BaseContext) ([]string, []string) {
	var cflags []string
	var includeDirs []string
	/************************************************
	* ** General Config
	* ***********************************************/
	cflags = append(cflags, "-DHAS_MDROID_BUILDCFG")
	includeDirs = append(includeDirs, "system/bt/mediatek/include")

	// MTK interop extension
	if mediatek.GetFeature("MTK_BT_INTEROP_EXTENSION") == "yes" {
		cflags = append(cflags, "-DMTK_INTEROP_EXTENSION=TRUE")
	}

	// MTK a2dp hal layer PCM dump
	cflags = append(cflags, "-DMTK_A2DP_PCM_DUMP=TRUE")

	// MTK support customized snoop log
	cflags = append(cflags, "-DMTK_STACK_CONFIG_LOG=TRUE")

	// Enable BLE VND Feature for MTK BT
	cflags = append(cflags, "-DBLE_VND_INCLUDED=TRUE")

	// MTK adjust a2dp packet length of SBC as 2-DH5/3-DH5 for firmware
	cflags = append(cflags, "-DMTK_VND_A2DP_PKT_LEN=TRUE")

	// MTK player application setttings
	if mediatek.GetFeature("MTK_BT_AVRCP_TG_APP_SETTINGS_SUPPORT") == "yes" {
		cflags = append(cflags, "-DMTK_AVRCP_APP_SETTINGS=TRUE")
	}

	// MTK modifies the AOSP for bug fixing
	cflags = append(cflags, "-DMTK_BT_PROPRIETARY_HANDLING=TRUE")

	return cflags, includeDirs
}
