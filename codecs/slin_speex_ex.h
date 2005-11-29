/*
 * Signed 16-bit audio data, 500ms of speech at 8kHz to ensure no DTX triggered
 *
 * Source: speex.example
 *
 * Copyright (C) 1999, Mark Spencer
 *
 * Distributed under the terms of the GNU General Public License
 *
 */

static signed short slin_speex_ex[] = {
0x4a00, 0xa700, 0x6d00, 0x4900, 0x5800, 0x3e00, 0x1b00, 0x1400, 0xf9ff, 0xe4ff, 0x0a00, 0xf0ff, 0xbbff, 0x0200, 0x0800, 0xd6ff,
0xf1ff, 0x1200, 0x0500, 0x1000, 0x0f00, 0x0700, 0x4100, 0x2800, 0xf4ff, 0x2900, 0x3100, 0xf4ff, 0xefff, 0xdaff, 0xdaff, 0xc2ff,
0xa5ff, 0xccff, 0xd4ff, 0xc7ff, 0xe6ff, 0x0600, 0x0c00, 0x1900, 0x1200, 0x0d00, 0x2900, 0x1e00, 0xf4ff, 0x0000, 0x1200, 0xf5ff,
0xe5ff, 0x0000, 0x0000, 0xf1ff, 0x0300, 0x0b00, 0xfdff, 0x0500, 0x0f00, 0x0600, 0x0d00, 0x0f00, 0x0000, 0xfdff, 0xfcff, 0xe5ff,
0xdcff, 0xe0ff, 0xafff, 0xb2ff, 0xe4ff, 0xceff, 0xe2ff, 0x0000, 0x1600, 0x2600, 0x2400, 0x3f00, 0x4a00, 0x3d00, 0x2b00, 0x2f00,
0x4300, 0x1000, 0x0300, 0x0700, 0xe9ff, 0xf3ff, 0xebff, 0xd9ff, 0xe5ff, 0xf9ff, 0xeeff, 0xe9ff, 0x0500, 0x0a00, 0x0300, 0x0800,
0x1800, 0x1c00, 0x1000, 0x1300, 0x0d00, 0x0400, 0x0200, 0xf9ff, 0xe4ff, 0xe6ff, 0xe0ff, 0xd9ff, 0xecff, 0xf2ff, 0xe8ff, 0xfdff,
0x1100, 0x0d00, 0x0b00, 0x1100, 0x1100, 0x1200, 0x0c00, 0x0400, 0x0900, 0x0d00, 0x0100, 0xfdff, 0x0000, 0xfdff, 0x0000, 0xffff,
0xf5ff, 0xfcff, 0x0600, 0xfeff, 0x0000, 0x0300, 0x1300, 0x1700, 0x0300, 0x0700, 0xf0ff, 0xe1ff, 0xd7ff, 0xd1ff, 0xc6ff, 0xc2ff,
0xf2ff, 0xf0ff, 0xecff, 0x0700, 0x3200, 0x2d00, 0x0400, 0x2200, 0x2900, 0x1900, 0x2400, 0x0500, 0x1900, 0x1f00, 0xfeff, 0x0d00,
0x0600, 0xebff, 0xf5ff, 0x0600, 0xebff, 0xe3ff, 0x0c00, 0x0d00, 0xefff, 0x1100, 0x3a00, 0x0700, 0x1700, 0x2f00, 0xf7ff, 0x0700,
0x2600, 0xd9ff, 0xb8ff, 0xeaff, 0xbeff, 0x9eff, 0xc7ff, 0xccff, 0xc4ff, 0xeeff, 0x0000, 0xfdff, 0x1200, 0x1d00, 0x3100, 0x3600,
0x2d00, 0x3c00, 0x4700, 0x3000, 0x2000, 0x2300, 0x1800, 0x0900, 0xffff, 0xeaff, 0xe7ff, 0xe8ff, 0xd3ff, 0xc5ff, 0xe1ff, 0xedff,
0xe4ff, 0xfaff, 0xe3ff, 0xacff, 0xcdff, 0xd4ff, 0xabff, 0xa6ff, 0xdaff, 0x0500, 0xf1ff, 0xfaff, 0x3800, 0x7000, 0x4100, 0x1c00,
0x4b00, 0x5300, 0x3a00, 0x2a00, 0x1400, 0x2000, 0x1c00, 0x1500, 0x0500, 0xf6ff, 0x0000, 0x1300, 0xfaff, 0xe9ff, 0x0000, 0x0a00,
0xfcff, 0xfaff, 0x1000, 0x1200, 0x0900, 0x0c00, 0x0400, 0x0000, 0x0800, 0xffff, 0xe7ff, 0xe2ff, 0xe6ff, 0xd5ff, 0xdaff, 0xe0ff,
0xdcff, 0xeeff, 0x0200, 0xfbff, 0x0000, 0x1000, 0x1100, 0x1500, 0x1900, 0x1400, 0x1900, 0x1c00, 0x1500, 0x1000, 0x0d00, 0x0b00,
0x0700, 0x0200, 0xf6ff, 0xf5ff, 0xf9ff, 0xecff, 0xe8ff, 0xeeff, 0xf4ff, 0xf4ff, 0xeeff, 0xecff, 0xebff, 0xd7ff, 0xd8ff, 0xeaff,
0xe0ff, 0xddff, 0x0100, 0x0500, 0xf8ff, 0x1800, 0x2d00, 0x1d00, 0x1a00, 0x2100, 0x1f00, 0x1f00, 0x1a00, 0x0f00, 0x0300, 0x0d00,
0x0e00, 0xfeff, 0x0000, 0xfeff, 0x0000, 0x0000, 0xf1ff, 0xfaff, 0x0a00, 0x0100, 0xf5ff, 0x0300, 0x0e00, 0x0300, 0x0200, 0x0000,
0x0800, 0x0700, 0xf8ff, 0xf5ff, 0xf1ff, 0xefff, 0xe9ff, 0xe0ff, 0xe0ff, 0xedff, 0xf0ff, 0xedff, 0xfcff, 0xffff, 0xfdff, 0x0a00,
0x0f00, 0x0e00, 0x0f00, 0x1700, 0x1400, 0x0700, 0x0500, 0x0800, 0x0000, 0x0200, 0x0400, 0xfaff, 0xffff, 0x0200, 0xfaff, 0xfbff,
0x0300, 0x0400, 0x0400, 0x0100, 0xe7ff, 0xd4ff, 0xdbff, 0xe2ff, 0xe0ff, 0xd0ff, 0xe7ff, 0x0700, 0xf2ff, 0x0000, 0x2d00, 0x2000,
0x1800, 0x2c00, 0x1f00, 0x2600, 0x2200, 0x0e00, 0x0e00, 0x0000, 0x0400, 0x0800, 0x0300, 0xfbff, 0x0500, 0x1d00, 0x0000, 0xfbff,
0x2e00, 0x2500, 0x0600, 0x1c00, 0x2600, 0x1600, 0x0e00, 0x0200, 0xf9ff, 0x0000, 0xe1ff, 0xc0ff, 0xd5ff, 0xc9ff, 0xb6ff, 0xbcff,
0xccff, 0xc9ff, 0xcbff, 0xe5ff, 0xeaff, 0xf0ff, 0xfeff, 0x0000, 0x0b00, 0x1f00, 0x2000, 0x1b00, 0x2000, 0x2400, 0x1800, 0x1200,
0x1700, 0x0c00, 0x0b00, 0x0d00, 0xfdff, 0xf5ff, 0x0000, 0xf9ff, 0xefff, 0xfaff, 0xf7ff, 0xf0ff, 0xe7ff, 0xcbff, 0xd1ff, 0xedff,
0xf5ff, 0xfcff, 0xfeff, 0x1e00, 0x3200, 0x2200, 0x3800, 0x4f00, 0x3800, 0x2c00, 0x4000, 0x3e00, 0x2e00, 0x2900, 0x1200, 0x0500,
0x0700, 0x0000, 0xf2ff, 0xf2ff, 0xf8ff, 0xf5ff, 0xf7ff, 0xeaff, 0xefff, 0x0100, 0xfcff, 0xf9ff, 0x0000, 0x0700, 0xf9ff, 0xf2ff,
0xf9ff, 0xf0ff, 0xf1ff, 0xe7ff, 0xddff, 0xe1ff, 0xddff, 0xd9ff, 0xdaff, 0xe1ff, 0xe2ff, 0xe5ff, 0xe5ff, 0xf0ff, 0xfaff, 0xf4ff,
0xfdff, 0x1100, 0x0e00, 0x0e00, 0x1800, 0x1600, 0x1200, 0x1400, 0x1500, 0x1200, 0x0b00, 0x0600, 0x0c00, 0x0500, 0x0100, 0x0b00,
0x0200, 0xfaff, 0xffff, 0xf9ff, 0xe8ff, 0xd0ff, 0xc9ff, 0xd5ff, 0xe0ff, 0xe6ff, 0xedff, 0xf5ff, 0x1100, 0x1300, 0x1f00, 0x3600,
0x2900, 0x2200, 0x2c00, 0x3700, 0x2d00, 0x2000, 0x1a00, 0x0800, 0xfaff, 0xfcff, 0xf6ff, 0xf2ff, 0xebff, 0xf7ff, 0x0200, 0xf1ff,
0xf5ff, 0x0d00, 0x0900, 0xf9ff, 0x1000, 0x2000, 0x0a00, 0xf9ff, 0xffff, 0xf9ff, 0xf2ff, 0xeeff, 0xdbff, 0xd8ff, 0xdbff, 0xdaff,
0xdaff, 0xdfff, 0xe6ff, 0xecff, 0xedff, 0xf0ff, 0x0500, 0x0900, 0x0400, 0x0f00, 0x2600, 0x2700, 0x2200, 0x2b00, 0x1700, 0x1100,
0x2a00, 0x2200, 0x0c00, 0x0200, 0x0100, 0x0200, 0xfeff, 0xfaff, 0xf7ff, 0xf3ff, 0xf1ff, 0xf5ff, 0xf6ff, 0xe8ff, 0xd3ff, 0xcdff,
0xdbff, 0xebff, 0xedff, 0xf0ff, 0xfbff, 0x1400, 0x1700, 0x2000, 0x3400, 0x2700, 0x1a00, 0x2100, 0x2e00, 0x2900, 0x1e00, 0x0c00,
0xfbff, 0xfbff, 0xf9ff, 0xf3ff, 0xf4ff, 0xe8ff, 0xecff, 0x0300, 0xf8ff, 0xf4ff, 0x0100, 0x0600, 0xf7ff, 0x0900, 0x1e00, 0x0f00,
0x0900, 0x0700, 0x0000, 0x0000, 0x0000, 0xf5ff, 0xebff, 0xefff, 0xf3ff, 0xeeff, 0xf0ff, 0xf1ff, 0xefff, 0xf0ff, 0xf6ff, 0xfcff,
0xf9ff, 0xf8ff, 0xfbff, 0x0300, 0x0900, 0x0800, 0x0900, 0x0100, 0x0200, 0x0c00, 0x0e00, 0x0300, 0xfdff, 0xfcff, 0xfbff, 0xfbff,
0xf9ff, 0xf3ff, 0xf3ff, 0xecff, 0xf6ff, 0xfbff, 0xf4ff, 0xecff, 0xd6ff, 0xdcff, 0xf1ff, 0xf1ff, 0xeeff, 0xf6ff, 0x0600, 0x0a00,
0x1000, 0x1e00, 0x1b00, 0x1100, 0x1800, 0x2800, 0x2500, 0x2600, 0x1c00, 0x0e00, 0x0900, 0x0e00, 0x0800, 0x0200, 0xfaff, 0xf5ff,
0x0300, 0xf8ff, 0xfcff, 0x0500, 0x0500, 0x0000, 0xfaff, 0x0b00, 0x1c00, 0x0d00, 0xfdff, 0x0200, 0x0100, 0xfbff, 0xf5ff, 0xe8ff,
0xe8ff, 0xecff, 0xdaff, 0xd1ff, 0xddff, 0xdeff, 0xd8ff, 0xdeff, 0xedff, 0xf3ff, 0xf8ff, 0x0100, 0x1100, 0x2300, 0x2800, 0x3100,
0x3200, 0x3500, 0x3a00, 0x3e00, 0x3400, 0x2700, 0x2100, 0x1400, 0x0900, 0xf7ff, 0xfaff, 0xf3ff, 0xe2ff, 0xeaff, 0xe3ff, 0xdfff,
0xd7ff, 0xc9ff, 0xdaff, 0xe4ff, 0xe0ff, 0xe4ff, 0xefff, 0xf4ff, 0xf6ff, 0x0000, 0x0100, 0x0000, 0x0600, 0x0c00, 0x0c00, 0x1000,
0x1900, 0x1600, 0x1500, 0x1600, 0x1100, 0x1400, 0x1200, 0x1300, 0x0900, 0x0d00, 0x1400, 0x0e00, 0x1900, 0x1400, 0x0900, 0x0600,
0x1600, 0x1900, 0x0800, 0x0300, 0x0000, 0xf5ff, 0xedff, 0xeaff, 0xdbff, 0xdcff, 0xcbff, 0xb7ff, 0xbfff, 0xc3ff, 0xc2ff, 0xbfff,
0xcbff, 0xdaff, 0xe7ff, 0xf9ff, 0x0600, 0x1b00, 0x2c00, 0x2900, 0x3d00, 0x4b00, 0x4700, 0x4b00, 0x4a00, 0x4300, 0x3700, 0x3300,
0x1f00, 0x1400, 0x1000, 0x0f00, 0x0c00, 0x0000, 0xf6ff, 0xe7ff, 0xe7ff, 0xdeff, 0xd3ff, 0xe1ff, 0xdeff, 0xd9ff, 0xe0ff, 0xe8ff,
0xf1ff, 0xeaff, 0xe8ff, 0xf5ff, 0xf7ff, 0xf6ff, 0xf7ff, 0xf4ff, 0xf8ff, 0xfcff, 0x0100, 0xfcff, 0xf7ff, 0x0100, 0x0600, 0x0100,
0x0700, 0x0200, 0x0c00, 0x1000, 0x0e00, 0x1c00, 0x1a00, 0x1a00, 0x1800, 0x2200, 0x1f00, 0x1600, 0x1700, 0x0b00, 0xfbff, 0xfcff,
0xeaff, 0xd6ff, 0xdfff, 0xcbff, 0xc0ff, 0xc0ff, 0xbcff, 0xc1ff, 0xc5ff, 0xd1ff, 0xdaff, 0xecff, 0xffff, 0xfeff, 0x1000, 0x2700,
0x2400, 0x2400, 0x3800, 0x3600, 0x3100, 0x3300, 0x2800, 0x2000, 0x1e00, 0x1d00, 0x0800, 0x0400, 0x1600, 0x0900, 0x0000, 0x0d00,
0x0100, 0xefff, 0xf1ff, 0xedff, 0xe0ff, 0xe6ff, 0xf5ff, 0xe1ff, 0xdaff, 0xf3ff, 0xf6ff, 0xeeff, 0xf1ff, 0xf9ff, 0xfbff, 0x0000,
0x0700, 0x0900, 0x0d00, 0x1c00, 0x1d00, 0x1b00, 0x1a00, 0x2500, 0x2500, 0x1a00, 0x1600, 0x0e00, 0x0c00, 0x0b00, 0x0500, 0x0100,
0x0200, 0x0000, 0xfbff, 0xfcff, 0xfdff, 0xfcff, 0xfcff, 0xf9ff, 0xf6ff, 0xf3ff, 0xeaff, 0xe6ff, 0xecff, 0xe8ff, 0xdfff, 0xe6ff,
0xddff, 0xdaff, 0xecff, 0xefff, 0xf2ff, 0xfdff, 0x0100, 0xffff, 0x0800, 0x1700, 0x1000, 0x1300, 0x2100, 0x1b00, 0x1800, 0x1a00,
0x1200, 0x1000, 0x2000, 0x1100, 0x0000, 0x0700, 0x0500, 0xfdff, 0xfcff, 0x0300, 0xf2ff, 0xeaff, 0xe1ff, 0xcdff, 0xd2ff, 0xe0ff,
0xe0ff, 0xd5ff, 0xdfff, 0xf3ff, 0xf7ff, 0xfcff, 0x1100, 0x1800, 0x1900, 0x2100, 0x2600, 0x2800, 0x2900, 0x2d00, 0x2400, 0x1b00,
0x1a00, 0x1000, 0x0800, 0x0500, 0xfcff, 0xf1ff, 0xf4ff, 0xf6ff, 0xf2ff, 0xf6ff, 0xffff, 0xffff, 0x0000, 0x0a00, 0x0a00, 0xfbff,
0x0200, 0x0700, 0xf9ff, 0xf2ff, 0xeaff, 0xedff, 0xdfff, 0xd9ff, 0xe3ff, 0xd8ff, 0xdbff, 0xddff, 0xe2ff, 0xefff, 0xf6ff, 0xffff,
0x0000, 0x0d00, 0x1500, 0x1600, 0x2200, 0x2800, 0x2700, 0x2900, 0x3700, 0x2d00, 0x2500, 0x2c00, 0x2900, 0x1100, 0x0600, 0xfdff,
0xedff, 0xeaff, 0xddff, 0xcfff, 0xc9ff, 0xc0ff, 0xa8ff, 0xa8ff, 0xbeff, 0xceff, 0xd6ff, 0xe1ff, 0xf4ff, 0x1000, 0x1100, 0x1e00,
0x3e00, 0x3c00, 0x3c00, 0x3d00, 0x3e00, 0x3300, 0x3000, 0x2b00, 0x1300, 0x0b00, 0x0600, 0xfaff, 0xf6ff, 0xf4ff, 0xe9ff, 0xedff,
0x0000, 0xf8ff, 0xfbff, 0x0400, 0x0300, 0x0a00, 0x1400, 0x1300, 0x0100, 0x0b00, 0x0400, 0xf9ff, 0xf8ff, 0xeeff, 0xe9ff, 0xdeff,
0xe1ff, 0xdaff, 0xd7ff, 0xe5ff, 0xd2ff, 0xd3ff, 0xe3ff, 0xe7ff, 0xedff, 0xf5ff, 0x0000, 0x0600, 0x1300, 0x1f00, 0x1c00, 0x2600,
0x2b00, 0x3100, 0x2d00, 0x3100, 0x2e00, 0x2600, 0x1900, 0x1100, 0x0400, 0xf3ff, 0xf3ff, 0xdcff, 0xdbff, 0xddff, 0xd2ff, 0xccff,
0xbbff, 0xc3ff, 0xe4ff, 0xf4ff, 0xf6ff, 0xfbff, 0x1600, 0x2100, 0x1600, 0x2000, 0x2500, 0x1b00, 0x2700, 0x2100, 0x1400, 0x0d00,
0x1300, 0x0800, 0xfbff, 0xf8ff, 0xecff, 0xeeff, 0x0000, 0xefff, 0xeeff, 0xfcff, 0x0000, 0xfeff, 0x0600, 0x0700, 0x0200, 0x1400,
0x1600, 0x0700, 0x0d00, 0x1400, 0x0e00, 0x0900, 0x0b00, 0x0500, 0xfaff, 0x0600, 0x0000, 0xf2ff, 0xfaff, 0xf6ff, 0xe9ff, 0xecff,
0xecff, 0xe2ff, 0xe7ff, 0xf3ff, 0xf6ff, 0xf4ff, 0x0000, 0x0000, 0xffff, 0x1400, 0x0f00, 0x0f00, 0x1500, 0x1400, 0x1100, 0x1200,
0x0c00, 0x0400, 0x0500, 0xffff, 0x0200, 0xfbff, 0xf5ff, 0xfeff, 0xf5ff, 0xe5ff, 0xe3ff, 0xe4ff, 0xe6ff, 0x0a00, 0xffff, 0xf3ff,
0x0900, 0x0d00, 0xfbff, 0x0700, 0x0c00, 0xffff, 0x1200, 0x1e00, 0xfdff, 0x0700, 0x1c00, 0x1200, 0x0f00, 0x1200, 0x1500, 0xfbff,
0x1200, 0x2300, 0xf9ff, 0x0300, 0x0d00, 0xf2ff, 0xf2ff, 0xf5ff, 0xd8ff, 0xd0ff, 0xddff, 0xd8ff, 0xc4ff, 0xdbff, 0xe0ff, 0xd6ff,
0xecff, 0xd8ff, 0xf7ff, 0x1100, 0xffff, 0x1500, 0x2a00, 0x1c00, 0x1800, 0x2200, 0x1500, 0x0800, 0x1d00, 0x1600, 0x0800, 0x1300,
0x0500, 0xecff, 0x1100, 0x0a00, 0x1700, 0x0900, 0xffff, 0x2700, 0x1b00, 0x0b00, 0x1400, 0x1200, 0xecff, 0x1000, 0x1800, 0xdfff,
0xfbff, 0xf1ff, 0x9bff, 0x90ff, 0x97ff, 0x81ff, 0xf2ff, 0xf9ff, 0x8cff, 0xcbff, 0x3300, 0x0200, 0x1300, 0x4f00, 0x1600, 0x2700,
0x7400, 0x4000, 0x2a00, 0x4e00, 0x4b00, 0x1800, 0x2b00, 0x3900, 0x1c00, 0x2500, 0x1000, 0xf1ff, 0xf9ff, 0xf5ff, 0xdbff, 0xdbff,
0x0000, 0x0b00, 0xd7ff, 0xcbff, 0xecff, 0xdfff, 0xebff, 0xe5ff, 0xafff, 0xdfff, 0xe5ff, 0xbeff, 0xc9ff, 0x0000, 0xe0ff, 0xcdff,
0x1d00, 0x1b00, 0xefff, 0x1c00, 0x2100, 0x1100, 0x3200, 0x3400, 0x1d00, 0x2d00, 0x2600, 0x3b00, 0x3600, 0x4200, 0x0a00, 0x0000,
0x5f00, 0x4000, 0x0000, 0x1000, 0x2800, 0xeaff, 0xd9ff, 0xcdff, 0xf8ff, 0x8400, 0xc1ff, 0xbefe, 0x56ff, 0x7fff, 0xe5fe, 0xf5ff,
0x3600, 0x4aff, 0x24ff, 0x6800, 0xdf00, 0x1600, 0x6d00, 0x7b00, 0xf200, 0xd500, 0x0700, 0x2300, 0x3500, 0x0600, 0xeaff, 0xd1ff,
0x95ff, 0xd5ff, 0x0300, 0xbcff, 0xe0ff, 0x0000, 0x2500, 0x3100, 0xdf00, 0x6700, 0x0300, 0x6900, 0x2a00, 0x3f00, 0x0400, 0x7dff,
0x58ff, 0xb2ff, 0x7bff, 0x71ff, 0xa1ff, 0x7dff, 0xd0ff, 0x1700, 0x0000, 0x0700, 0x5900, 0x4b00, 0x4b00, 0x4700, 0xf8ff, 0x4300,
0x5e00, 0x0d00, 0xdaff, 0x1300, 0x0e00, 0x0f00, 0x2d00, 0xeaff, 0x1400, 0x6200, 0x3a00, 0xe3ff, 0x0c00, 0x1600, 0xfbff, 0xe8ff,
0xfbff, 0x6700, 0xe3ff, 0x8eff, 0xf2fe, 0x25fe, 0x9bfe, 0x54ff, 0x94ff, 0x94ff, 0x7dff, 0x3000, 0x9401, 0xce01, 0x5601, 0x0e01,
0xd300, 0x4f01, 0xdf00, 0x6fff, 0x25ff, 0x79ff, 0x5eff, 0x51ff, 0xd4ff, 0xbaff, 0x1b00, 0xbe00, 0x4a00, 0xf3ff, 0x2800, 0x4500,
0x5e00, 0x1800, 0x84ff, 0x8aff, 0xc7ff, 0xc2ff, 0x8aff, 0xd7fe, 0x16ff, 0x0000, 0x1300, 0xf7ff, 0xbcff, 0xc3ff, 0x5500, 0x7c00,
0x5a00, 0x5400, 0x2d00, 0x2d00, 0x2b00, 0xf9ff, 0xfcff, 0x0500, 0xddff, 0x1d00, 0x6600, 0x7a00, 0x9200, 0x3700, 0x0e00, 0x5600,
0x2500, 0xcfff, 0xfdff, 0xe3ff, 0xc5ff, 0xcdff, 0x00ff, 0x67ff, 0xb100, 0x3200, 0x3200, 0xc800, 0xcdff, 0xaaff, 0x4dff, 0x42fd,
0xc4fd, 0x3500, 0x2a00, 0x99ff, 0x1d00, 0xe500, 0x4e02, 0xf802, 0x6f01, 0xd4ff, 0x4f00, 0x7300, 0x88ff, 0x20ff, 0x81fe, 0x9afe,
0xb2ff, 0x6800, 0x9100, 0x9700, 0x3001, 0x5500, 0xf4ff, 0xa400, 0xb2ff, 0xdeff, 0x1500, 0x05ff, 0x4fff, 0xe6ff, 0xaaff, 0xc9ff,
0x0000, 0xafff, 0xeeff, 0x8200, 0x1800, 0xc7ff, 0xf1ff, 0xd5ff, 0x3000, 0x1d00, 0x50ff, 0x89ff, 0x82ff, 0x7cff, 0x2000, 0x1d00,
0x0d00, 0x9500, 0x8c00, 0x2d00, 0x5b00, 0x2200, 0xf8ff, 0x6700, 0x7800, 0x3e00, 0xa800, 0x9100, 0xbfff, 0xccff, 0xdaff, 0x9bff,
0x8400, 0x98ff, 0xa4fe, 0xddff, 0x1200, 0x2e00, 0xf100, 0x3500, 0x73ff, 0x99ff, 0xd1fd, 0x97fd, 0x0500, 0x7800, 0x3400, 0xc700,
0xfc00, 0x8201, 0x6002, 0x8201, 0xb5ff, 0xaeff, 0xa5ff, 0x15ff, 0x90ff, 0x53ff, 0xccfe, 0x8eff, 0x9d00, 0xef00, 0x0601, 0xf300,
0xa4ff, 0x5cff, 0x0b00, 0xfcff, 0x0d00, 0xd9ff, 0x3cff, 0x60ff, 0x5400, 0x3400, 0x2b00, 0x4800, 0xbbff, 0x0300, 0x5a00, 0xf5ff,
0xc7ff, 0xe5ff, 0x91ff, 0xfeff, 0x0000, 0x51ff, 0xa8ff, 0x57ff, 0x74ff, 0x3a00, 0x6500, 0x5400, 0x0e00, 0x2900, 0x3800, 0x3d00,
0x3300, 0x1100, 0x4400, 0x7200, 0x5c00, 0x6f00, 0x4c00, 0xc8ff, 0xa6ff, 0xc4ff, 0x9bff, 0x6a00, 0x1000, 0xccfe, 0xc6ff, 0x7e00,
0x8aff, 0x6b00, 0xae00, 0x7bff, 0x71ff, 0xa0fd, 0x05fd, 0xc5ff, 0x5001, 0x1801, 0x9a01, 0x3f01, 0x1701, 0xc102, 0xd901, 0x83ff,
0x35ff, 0xa7fe, 0x52fe, 0x98ff, 0x9eff, 0xcbfe, 0x90ff, 0xe100, 0x4801, 0xa001, 0x5101, 0x59ff, 0x04ff, 0xc3ff, 0x1800, 0x4400,
0xe7ff, 0x22ff, 0x1dff, 0x4000, 0x2c00, 0x4b00, 0x7800, 0xa3ff, 0xd9ff, 0x7b00, 0x1a00, 0xb7ff, 0xb4ff, 0x70ff, 0xb6ff, 0x2200,
0xc9ff, 0x35ff, 0xaeff, 0x2b00, 0x4700, 0x5100, 0xe6ff, 0x1100, 0x5900, 0x0500, 0xf0ff, 0x2300, 0x5b00, 0xbd00, 0xec00, 0xc500,
0x8200, 0x93ff, 0x39ff, 0xafff, 0xbfff, 0x5200, 0x9cff, 0x49fe, 0x7fff, 0xa600, 0xfcff, 0xaa00, 0x7000, 0xf8fe, 0x64fe, 0xf7fc,
0x73fd, 0x2100, 0xa001, 0xe501, 0x4a02, 0x3701, 0xe500, 0x7402, 0x6601, 0xb5ff, 0x5eff, 0x69fe, 0x40fe, 0xc8ff, 0xe6ff, 0x1cff,
0xd1ff, 0xc900, 0x6601, 0xdf01, 0xf300, 0x67ff, 0xdcfe, 0x50ff, 0x2500, 0x7400, 0x0900, 0x42ff, 0x32ff, 0xe2ff, 0x2e00, 0x8700,
0x2800, 0x84ff, 0xd4ff, 0x5a00, 0x1000, 0xc1ff, 0xafff, 0x62ff, 0xa7ff, 0x2700, 0x1100, 0xc5ff, 0xe2ff, 0xeeff, 0xb8ff, 0x3200,
0x8b00, 0x0d00, 0x2600, 0x7b00, 0xf4ff, 0x6dff, 0x3d00, 0x9600, 0x8800, 0x2201, 0xa500, 0xf1ff, 0xe7ff, 0x9fff, 0x70ff, 0xe9ff,
0x3a00, 0x5fff, 0xe8fe, 0x52ff, 0x0f00, 0x8e00, 0xb300, 0x3800, 0x1aff, 0x77fe, 0x13fd, 0x9afc, 0x6dff, 0xe301, 0x5602, 0x9902,
0x3b01, 0xa6ff, 0x1501, 0x1a02, 0xa300, 0xebff, 0x1aff, 0xe6fd, 0x09ff, 0x6700, 0x5800, 0xefff, 0x4500, 0xd100, 0x6601, 0x5301,
0x5600, 0x0eff, 0x9dfe, 0x91ff, 0x2400, 0x8500, 0xd7ff, 0xeffe, 0x39ff, 0xddff, 0x8c00, 0xa500, 0x0c00, 0x86ff, 0xd0ff, 0x2f00,
0x1200, 0xf1ff, 0xbfff, 0x88ff, 0xd1ff, 0x4900, 0x2100, 0xbcff, 0xb1ff, 0x9aff, 0xf1ff, 0x8300, 0x6400, 0x3b00, 0x3e00, 0x2400,
0xa3ff, 0x0000, 0x8200, 0x4b00, 0xa800, 0xe600, 0x0300, 0x2cff, 0x82ff, 0x0b00, 0x4300, 0x5600, 0xa9ff, 0xcdfe, 0x15ff, 0x6700,
0xed00, 0x8a00, 0xe3ff, 0x74fe, 0xe3fc, 0xd1fc, 0x98fe, 0x2601, 0x7b02, 0x8d02, 0x8001, 0x3f00, 0xd300, 0x9501, 0xfe00, 0xeeff,
0x50ff, 0x67fe, 0x81fe, 0xe8ff, 0x4900, 0x1200, 0x6800, 0xc000, 0xc200, 0x0c01, 0xb100, 0x7bff, 0x2fff, 0x91ff, 0xd6ff, 0xfeff,
0xfeff, 0x63ff, 0x35ff, 0x0700, 0x7100, 0x6600, 0x4400, 0xceff, 0x9cff, 0x1100, 0x4500, 0xdfff, 0xabff, 0x96ff, 0x96ff, 0x1000,
0x5b00, 0x0f00, 0xc2ff, 0xe3ff, 0xfdff, 0x3a00, 0x9000, 0x4c00, 0x1400, 0xe6ff, 0xe2ff, 0xc0ff, 0xf3ff, 0x4300, 0x1a00, 0x5100,
0x7300, 0x3100, 0xc2ff, 0xebff, 0x5a00, 0x1e00, 0xafff, 0x3aff, 0x3cff, 0x80ff, 0x5a00, 0xf500, 0xffff, 0x57ff, 0x39ff, 0xe8fd,
0x65fd, 0x65ff, 0x7d01, 0xfb01, 0x5b02, 0x7501, 0x9aff, 0x6000, 0x7701, 0x6b00, 0x94ff, 0x31ff, 0xe2fd, 0x85fe, 0x9300, 0xab00,
0x4900, 0x9300, 0x5900, 0x3b00, 0x0701, 0x9000, 0x47ff, 0x4eff, 0x6cff, 0xedff, 0x5c00, 0x0000, 0x33ff, 0x81ff, 0x6200, 0x7400,
0x9b00, 0x3400, 0x67ff, 0x94ff, 0x3100, 0x2800, 0xfeff, 0xfdff, 0xa3ff, 0x9fff, 0x2100, 0x2400, 0xebff, 0xf5ff, 0xe4ff, 0xe0ff,
0x3f00, 0x5100, 0x1600, 0x1a00, 0xe7ff, 0xe0ff, 0xd4ff, 0x0000, 0x1200, 0x0500, 0x5b00, 0xc200, 0x5000, 0xcbff, 0x3b00, 0xe5ff,
0x9fff, 0xf0ff, 0xd4ff, 0x76ff, 0x40ff, 0x2200, 0x8c00, 0x0f00, 0x5700, 0x5fff, 0xdffc, 0xfbfc, 0xe8ff, 0xc501, 0xa202, 0xbc02,
0x6300, 0x49ff, 0x0b01, 0x7001, 0x6f00, 0xe2ff, 0x72fe, 0x84fd, 0x7dff, 0xcc00, 0x7300, 0x7c00, 0x5e00, 0xf1ff, 0xa200, 0x2101,
0xd4ff, 0x46ff, 0x5eff, 0x82ff, 0x4800, 0x7d00, 0xd2ff, 0x71ff, 0xe3ff, 0x1b00, 0x7b00, 0x5c00, 0xc4ff, 0x84ff, 0x76ff, 0xe5ff,
0x2b00, 0x3500, 0x1500, 0xe2ff, 0x99ff, 0x73ff, 0x9bff, 0xc7ff, 0xc8ff, 0x0000, 0x2d00, 0x4100, 0x3400, 0x2600, 0xf1ff, 0xd6ff,
0x0900, 0x1300, 0xe0ff, 0x5c00, 0xbc00, 0x4700, 0x2500, 0xd5ff, 0x5eff, 0xb5ff, 0x4800, 0x3200, 0x0a00, 0x5bff, 0x91ff, 0x9b00,
0x7000, 0x6400, 0x0800, 0xa7fd, 0x41fc, 0x8dfe, 0x3c01, 0x0102, 0x4b03, 0x0002, 0x58ff, 0x8b00, 0xc401, 0xad00, 0x5c00, 0x7aff,
0x8efd, 0x85fe, 0x5600, 0x4000, 0x5700, 0xd200, 0xa700, 0xd400, 0xbb01, 0x7500, 0x1bff, 0x11ff, 0x47ff, 0x0700, 0x7500, 0x3c00,
0x67ff, 0x05ff, 0x91ff, 0x4300, 0x6700, 0x3300, 0x1200, 0x94ff, 0x8dff, 0x1e00, 0xd6ff, 0x98ff, 0xafff, 0x9bff, 0x55ff, 0x97ff,
0xaeff, 0x86ff, 0xc1ff, 0x2f00, 0x5200, 0x7100, 0x7600, 0xfdff, 0x9fff, 0xc3ff, 0xe5ff, 0xd9ff, 0x1500, 0x7e00, 0x3c00, 0x8200,
0x5900, 0x60ff, 0x8cff, 0x1e00, 0x3400, 0x4300, 0x3800, 0x1dff, 0xbffe, 0xd2ff, 0xcf00, 0xff00, 0x5c00, 0x36ff, 0xd4fd, 0x85fc,
0x18fe, 0xa301, 0x2e02, 0xfc01, 0x9d02, 0xf400, 0x4400, 0xbf01, 0xea00, 0x40ff, 0x83ff, 0x05ff, 0x5afe, 0xc5ff, 0x5100, 0xa4ff,
0x8f00, 0x8501, 0x2501, 0x2e01, 0xd300, 0x21ff, 0xe0fe, 0xd1ff, 0x0400, 0x5a00, 0xd2ff, 0x62ff, 0xcfff, 0xc0ff, 0xfbff, 0x2b00,
0xf9ff, 0xd6ff, 0xf7ff, 0xfbff, 0xb7ff, 0xc6ff, 0x2eff, 0x52ff, 0x3900, 0x1500, 0xc2ff, 0xf0ff, 0xc6ff, 0xccff, 0x7f00, 0x9500,
0x0200, 0x0b00, 0x0000, 0x98ff, 0xa3ff, 0xb9ff, 0xacff, 0x4600, 0x8a00, 0x9300, 0x1300, 0x94ff, 0xd4ff, 0x2a00, 0x1301, 0xce00,
0x78ff, 0xa7fe, 0xa1fe, 0x76ff, 0xe200, 0xfe00, 0xecff, 0xadff, 0x6fff, 0xe4fd, 0x2afd, 0x51ff, 0x3f01, 0x8c01, 0x9202, 0xac01,
0xf4ff, 0x0d01, 0x7601, 0x0800, 0xd5ff, 0xafff, 0x9cfe, 0x2dff, 0x2200, 0x85ff, 0xbdff, 0xd500, 0xc000, 0xd300, 0x2701, 0x0700,
0x12ff, 0xb6ff, 0xf5ff, 0xebff, 0x7900, 0xc7ff, 0x4dff, 0x5c00, 0x9800, 0xf9ff, 0xa4ff, 0xa4ff, 0x6bff, 0xafff, 0x2a00, 0xceff,
0xc2ff, 0xc6ff, 0xb9ff, 0x3600, 0x5a00, 0xdbff, 0xa1ff, 0xe1ff, 0x1500, 0x3e00, 0x5d00, 0x0400, 0xccff, 0x2000, 0x1900, 0xa7ff,
0x77ff, 0x9dff, 0xfdff, 0x4a00, 0xcd00, 0x0300, 0x8bff, 0x0000, 0xf0ff, 0xa100, 0xbf00, 0xdaff, 0x04ff, 0x2aff, 0x76ff, 0xe1ff,
0x8b00, 0x8100, 0x3700, 0x4700, 0x2fff, 0x1ffd, 0xd7fd, 0x6a00, 0x6c01, 0x9301, 0x9c01, 0x5900, 0x3c00, 0x9f01, 0x6201, 0xbfff,
0x28ff, 0x69ff, 0x63ff, 0xf5ff, 0x0a00, 0x2cff, 0x91ff, 0x9e00, 0x1c01, 0x0d01, 0x7900, 0xa9ff, 0x3aff, 0x2b00, 0x3500, 0xc4ff,
0xc5ff, 0x77ff, 0xb8ff, 0x9600, 0xaa00, 0xd4ff, 0x97ff, 0xdfff, 0xc3ff, 0x1a00, 0xf9ff, 0x65ff, 0x81ff, 0xa7ff, 0xd0ff, 0x2a00,
0x0600, 0x0800, 0x2d00, 0xebff, 0x2000, 0x6d00, 0x6a00, 0x5900, 0x5700, 0x0c00, 0x85ff, 0x9bff, 0xe4ff, 0x0400, 0x4000, 0xd7ff,
0x0f00, 0x1f00, 0xf2ff, 0xb800, 0x6c00, 0xacff, 0x59ff, 0x2eff, 0x5fff, 0x1f00, 0x5200, 0x1f00, 0x2700, 0x3e00, 0xcdfe, 0x09fd,
0x87fd, 0x2300, 0x0802, 0x8401, 0x2f01, 0x7c00, 0x9c00, 0x0302, 0xb301, 0x8fff, 0xc2fe, 0x25ff, 0x0000, 0x6e00, 0xabff, 0xedfe,
0x54ff, 0xa800, 0xa301, 0x4801, 0x1900, 0x8eff, 0xf2ff, 0x7000, 0x6c00, 0xc6ff, 0x02ff, 0x34ff, 0x0000, 0x6c00, 0x4f00, 0x55ff,
0x65ff, 0x3c00, 0x4d00, 0x1d00, 0x81ff, 0x49ff, 0xaaff, 0x2300, 0x1600, 0xaaff, 0x92ff, 0xb7ff, 0x7100, 0x4900, 0xf8ff, 0x4900,
0x5700, 0x9f00, 0xa100, 0x5b00, 0xb6ff, 0x8bff, 0x0700, 0x3300, 0x3e00, 0xa2ff, 0x99ff, 0x2e00, 0x1200, 0x9500, 0x9200, 0xceff,
0x99ff, 0xa9ff, 0x77ff, 0x5fff, 0xc7ff, 0x1c00, 0x1b00, 0x5400, 0x9fff, 0xd8fd, 0xe8fc, 0xaefe, 0x7301, 0x8a01, 0xd700, 0x6b00,
0x6600, 0xfa01, 0x9d02, 0x5e00, 0xddfe, 0x75ff, 0x1f00, 0x8500, 0x6dff, 0x3ffe, 0xc4fe, 0x7200, 0x5401, 0xe600, 0x2900, 0xd8ff,
0x6900, 0xe800, 0x9400, 0xd8ff, 0x45ff, 0x6aff, 0xedff, 0xf4ff, 0xf3ff, 0x8cff, 0x7aff, 0x1e00, 0x3a00, 0xfcff, 0xc7ff, 0xbbff,
0xd1ff, 0x1300, 0x1c00, 0xc1ff, 0xbfff, 0xd6ff, 0x1300, 0x5100, 0xf7ff, 0x77ff, 0xe4ff, 0x9000, 0xa400, 0x9a00, 0x5300, 0xacff,
0xa5ff, 0x3a00, 0x3800, 0x0000, 0xe2ff, 0xe3ff, 0x4300, 0xc400, 0x5e00, 0x90ff, 0xb4ff, 0x3a00, 0xf6ff, 0x8eff, 0x86ff, 0xcbfe,
0x49ff, 0xe400, 0x7f00, 0x5cff, 0x80fe, 0x61fd, 0x78fe, 0xf601, 0x4001, 0xabff, 0xc500, 0xe000, 0xf201, 0x0c03, 0x7e00, 0x6ffe,
0xceff, 0x7100, 0xd0ff, 0x6dff, 0x5cfe, 0x4afe, 0x5800, 0x6d01, 0x0e00, 0xc7ff, 0x2e00, 0x2a00, 0xc800, 0xe500, 0x89ff, 0x3cff,
0x5900, 0x1a00, 0xcfff, 0x0b00, 0x71ff, 0x87ff, 0x7000, 0x3100, 0x95ff, 0xe1ff, 0x0000, 0xe9ff, 0x4700, 0x0300, 0x7aff, 0xf1ff,
0x3700, 0xc9ff, 0xfcff, 0x0000, 0xd0ff, 0x6800, 0x9500, 0xdfff, 0xfaff, 0xe7ff, 0xe9ff, 0x9600, 0x1700, 0xbbff, 0x0b00, 0x5300,
0x9000, 0x9c00, 0x1900, 0x97ff, 0xd7ff, 0x6100, 0x2400, 0xbdfe, 0xe0fe, 0xb0ff, 0xb0ff, 0x6400, 0x5500, 0x0dff, 0xdafe, 0x38fe,
0xe1fd, 0x6200, 0x3801, 0xeaff, 0xe200, 0xf901, 0xd901, 0x4802, 0x8001, 0x25ff, 0xd7ff, 0xfa00, 0x7bff, 0xc8fe, 0xe4fe, 0x76fe,
0xb4ff, 0xa800, 0xd5ff, 0xa7ff, 0x4d00, 0x6d00, 0x6e00, 0xb600, 0xe3ff, 0xc3ff, 0x6600, 0x1b00, 0xa3ff, 0xc0ff, 0xbaff, 0x90ff,
0x0a00, 0x1c00, 0x99ff, 0x0900, 0x5000, 0xd6ff, 0x0e00, 0x7400, 0x1f00, 0xf5ff, 0x0d00, 0xc3ff, 0xe6ff, 0x4400, 0xebff, 0x9dff,
0xdfff, 0x5a00, 0x1c00, 0x80ff, 0xd4ff, 0x1f00, 0x1200, 0x4e00, 0x6f00, 0xd900, 0x7d00, 0x9aff, 0xdbff, 0x4000, 0x2300, 0xd1ff,
0xa3ff, 0x89ff, 0x74ff, 0xbdff, 0xb5ff, 0xe8ff, 0x4900, 0x0b00, 0xc6ff, 0x5aff, 0x6dfe, 0xc6fe, 0x3400, 0x6700, 0x2300, 0x9500,
0xa300, 0x2901, 0x0a02, 0xf200, 0x7fff, 0x3800, 0x7e00, 0xedff, 0xb8ff, 0x0fff, 0xeafe, 0xdeff, 0x4500, 0xdcff, 0xcaff, 0x0d00,
0x3d00, 0x7b00, 0x7c00, 0xffff, 0xf5ff, 0x2600, 0x1500, 0xe2ff, 0xd8ff, 0xc6ff, 0xb8ff, 0xf7ff, 0xf6ff, 0xd4ff, 0xf7ff, 0x1300,
0xf7ff, 0x0800, 0xf3ff, 0xbbff, 0xb5ff, 0xfcff, 0xcaff, 0xc4ff, 0xfcff, 0xd2ff, 0xfeff, 0x2500, 0x2000, 0x0400, 0x0000, 0x0200,
0x1000, 0x1e00, 0x1900, 0x2a00, 0x1600, 0x2e00, 0x5500, 0x1b00, 0x1f00, 0x1200, 0x2600, 0x6600, 0x3700, 0xfbff, 0x2200, 0x1300,
0xffff, 0xfbff, 0xcaff, 0x97ff, 0x9eff, 0x95ff, 0x9aff, 0xceff, 0xa2ff, 0x6fff, 0xabff, 0x0000, 0x2400, 0x0c00, 0xf2ff, 0x0f00,
0x6100, 0x7500, 0x4200, 0xfeff, 0x1c00, 0x5a00, 0x4a00, 0x2300, 0x0b00, 0x0f00, 0x0900, 0x1900, 0xfcff, 0xe0ff, 0xfdff, 0x1200,
0x1100, 0x1200, 0x1700, 0x1800, 0x1500, 0x1500, 0x0500, 0xf7ff, 0xf9ff, 0x0800, 0xffff, 0x0300, 0x0000, 0xccff, 0xdaff, 0x0200,
0xe5ff, 0xa7ff, 0x9cff, 0xb4ff, 0xd0ff, 0xcaff, 0xdbff, 0xc9ff, 0xd3ff, 0x0a00, 0x0500, 0x0600, 0x1500, 0x1400, 0x2d00, 0x5900,
0x3000, 0x0900, 0x1300, 0x0800, 0x2000, 0x3600, 0x0500, 0x0000, 0xf6ff, 0x0700, 0x1900, 0xf5ff, 0x1000, 0x1300, 0xcdff, 0xbdff,
0xd7ff, 0xe0ff, 0xd6ff, 0xe6ff, 0xbfff, 0xdfff, 0x3500, 0x1700, 0x0000, 0x2900, 0x4c00, 0x4600, 0x4500, 0x4a00, 0x2100, 0x3c00,
0x4000, 0x0a00, 0xeeff, 0xedff, 0xe6ff, 0xe1ff, 0xe0ff, 0xeeff, 0x0100, 0x0a00, 0x1300, 0x0900, 0x0500, 0x0c00, 0x0600, 0xf2ff,
0xecff, 0xe1ff, 0xe3ff, 0xe2ff, 0xe6ff, 0xe6ff, 0xf8ff, 0x0500, 0x0200, 0x0800, 0xe2ff, 0xdfff, 0x0000, 0x0300, 0xffff, 0x0000,
0x1200, 0x1300, 0x0600, 0x0c00, 0xfcff, 0x1200, 0x1000, 0xfcff, 0x0800, 0x0900, 0xfbff, 0xf6ff, 0xfeff, 0xfbff, 0xfeff, 0xf1ff,
0xfbff, 0x0400, 0x0600, 0x1700, 0x2500, 0x0e00, 0xdbff, 0xb8ff, 0xc9ff, 0xcfff, 0xb2ff, 0xbeff, 0xcdff, 0xc7ff, 0xe2ff, 0x0500,
0x2400, 0x1600, 0x2300, 0x3900, 0x4c00, 0x5400, 0x4200, 0x3a00, 0x3100, 0x3e00, 0x2e00, 0x0800, 0xf6ff, 0xf2ff, 0xf7ff, 0xfaff,
0xf7ff, 0xf3ff, 0x0600, 0x0f00, 0x0300, 0xffff, 0xf9ff, 0xe6ff, 0x0a00, 0x1f00, 0x0a00, 0x0300, 0xf8ff, 0xecff, 0xf2ff, 0xfdff,
0xccff, 0xbbff, 0xe5ff, 0xe6ff, 0xb3ff, 0xc7ff, 0x0000, 0xfaff, 0xf2ff, 0x0d00, 0x1000, 0x0e00, 0x2300, 0x1c00, 0x0f00, 0x1c00,
0x1700, 0x0b00, 0x1000, 0x0c00, 0x0400, 0x0100, 0x0200, 0x0300, 0x0000, 0xfdff, 0x0300, 0x0500, 0x0700, 0x0a00, 0x0600, 0x0400,
0x0100, 0xf5ff, 0xecff, 0xf2ff, 0xf5ff, 0xecff, 0x0200, 0xfdff, 0xe0ff, 0x0800, 0x1b00, 0x0300, 0x0000, 0x0700, 0x0500, 0x0400,
0x0100, 0xf0ff, 0xefff, 0x0100, 0xfcff, 0xfaff, 0x0700, 0x0600, 0x0e00, 0x1500, 0x0800, 0x0500, 0x0d00, 0x1100, 0x0800, 0x0a00,
0x0600, 0xf8ff, 0xfdff, 0x0400, 0x2200, 0x2800, 0x1500, 0x0f00, 0x1800, 0x0e00, 0xfcff, 0xf9ff, 0xefff, 0xe5ff, 0xc5ff, 0xb4ff,
0xc3ff, 0xcfff, 0xcbff, 0xcdff, 0xe4ff, 0xf7ff, 0x0300, 0x1300, 0xffff, 0xf8ff, 0x0e00, 0x0900, 0xfeff, 0xfeff, 0xfeff, 0xfbff,
0x0300, 0x0a00, 0x0a00, 0x0f00, 0x1100, 0x0d00, 0x0800, 0x0e00, 0x1600, 0x0d00, 0x0900, 0x0900, 0x0100, 0x0200, 0x0000, 0xebff,
0xfbff, 0x0200, 0xdeff, 0xf8ff, 0x0c00, 0xf8ff, 0xefff, 0x0100, 0x0000, 0x0200, 0x0500, 0x0f00, 0x1900, 0x2400, 0x2d00, 0x1b00,
0x1500, 0x1b00, 0x1e00, 0x1200, 0x0100, 0x0000, 0xfeff, 0x0200, 0x0400, 0xfeff, 0xfaff, 0x0200, 0x0b00, 0x1d00, 0x2e00, 0x0100,
0xf7ff, 0x0d00, 0x0300, 0xf7ff, 0xfbff, 0xf5ff, 0xeaff, 0xe8ff, 0xc9ff, 0xccff, 0xe2ff, 0xe2ff, 0xdeff, 0xe5ff, 0xf0ff, 0xfaff,
0x0900, 0x0700, 0xe8ff, 0xedff, 0x0300, 0xfdff, 0xf3ff, 0xf8ff, 0x0e00, 0x0b00, 0x1800, 0x1200, 0x0100, 0x0f00, 0x1300, 0x1100,
0x0e00, 0x1700, 0x1500, 0x0800, 0x0500, 0xf1ff, 0xe5ff, 0xe3ff, 0xd9ff, 0xceff, 0xc6ff, 0xbeff, 0xe5ff, 0xf2ff, 0xebff, 0xf1ff,
0x0200, 0xffff, 0x0900, 0x1000, 0x1000, 0x1300, 0x2a00, 0x3700, 0x2c00, 0x3000, 0x3000, 0x2300, 0x2100, 0x2500, 0x1d00, 0x1700,
0x1b00, 0x1800, 0x1800, 0x1900, 0x0f00, 0x0a00, 0x1300, 0x1e00, 0x1600, 0x0f00, 0xffff, 0x0200, 0xf2ff, 0xe3ff, 0xdfff, 0xdbff,
0xd5ff, 0xc2ff, 0xb8ff, 0xb3ff, 0xbaff, 0xc4ff, 0xc9ff, 0xd1ff, 0xe4ff, 0xfcff, 0xfbff, 0xfeff, 0x0c00, 0x0c00, 0x1300, 0x2200,
0x2300, 0x2100, 0x2b00, 0x3700, 0x2c00, 0x2d00, 0x2d00, 0x2300, 0x2800, 0x2300, 0x1300, 0x1800, 0x1000, 0xedff, 0xe1ff, 0xd6ff,
0xbcff, 0xb8ff, 0xb3ff, 0x98ff, 0x9bff, 0xb7ff, 0xcfff, 0xd6ff, 0xe5ff, 0xecff, 0xf5ff, 0x0000, 0x0600, 0x1600, 0x1100, 0x1a00,
0x3100, 0x3700, 0x3800, 0x3700, 0x3b00, 0x3900, 0x3700, 0x3200, 0x3400, 0x3600, 0x2e00, 0x2800, 0x2700, 0x1e00, 0x1800, 0x1800,
0x1000, 0x1400, 0x0900, 0xfcff, 0xf7ff, 0xf1ff, 0xe4ff, 0xd7ff, 0xd7ff, 0xd2ff, 0xc1ff, 0xc1ff, 0xc8ff, 0xc3ff, 0xc5ff, 0xd6ff,
0xddff, 0xdfff, 0xf0ff, 0x0000, 0x0300, 0x0300, 0x0900, 0x0a00, 0x1700, 0x2200, 0x1700, 0x2000, 0x3100, 0x3200, 0x3300, 0x2e00,
0x2800, 0x1d00, 0x2600, 0x2200, 0x1800, 0x1300, 0x0200, 0xf0ff, 0xd3ff, 0xc8ff, 0xc0ff, 0xbbff, 0xbbff, 0xb4ff, 0xaaff, 0xbaff,
0xd7ff, 0xe9ff, 0xf2ff, 0xfcff, 0x0500, 0x1000, 0x1900, 0x1a00, 0x2100, 0x1b00, 0x1d00, 0x2a00, 0x1d00, 0x1000, 0x0b00, 0x0c00,
0x0b00, 0x0600, 0x0100, 0x0400, 0x0900, 0x0900, 0x0600, 0x0500, 0x0800, 0x0f00, 0x1100, 0xfcff, 0x0900, 0x0c00, 0x0400, 0x0500,
0x0000, 0xf8ff, 0xf3ff, 0xf7ff, 0xefff, 0xe7ff, 0xeeff, 0xf1ff, 0xf2ff, 0xf5ff, 0xfbff, 0xfdff, 0xffff, 0x0000, 0x0100, 0x0000,
0xfeff, 0xffff, 0x0000, 0x0000, 0x0100, 0xffff, 0x0000, 0x0400, 0x0500, 0x0200, 0xffff, 0x0000, 0xfeff, 0xfcff, 0xfbff, 0xf7ff,
0xf3ff, 0xf1ff, 0xf6ff, 0xf1ff, 0xf4ff, 0xf7ff, 0xfaff, 0xfeff, 0x0700, 0x0e00, 0x1300, 0x1e00, 0x2000, 0x2200, 0x2000, 0x2200,
0x1d00, 0x1c00, 0x1d00, 0x1b00, 0x1600, 0x1b00, 0x1900, 0x1200, 0x0d00, 0x0a00, 0x0500, 0x0000, 0xebff, 0xddff, 0xe1ff, 0xe0ff,
0xddff, 0xdcff, 0xdeff, 0xdeff, 0xe6ff, 0xebff, 0xeeff, 0xf4ff, 0xffff, 0x0100, 0x0a00, 0x0b00, 0x0100, 0x0600, 0x1600, 0x0e00,
0x0a00, 0x1100, 0x1500, 0x1000, 0x1000, 0x0d00, 0x0b00, 0x0900, 0x0500, 0xfbff, 0xffff, 0x0200, 0x0000, 0x0100, 0x0100, 0x0200,
0xfbff, 0x0000, 0xfaff, 0xf4ff, 0xedff, 0xedff, 0xe9ff, 0xe4ff, 0xe2ff, 0xdeff, 0xdbff, 0xe0ff, 0xe0ff, 0xe3ff, 0xe9ff, 0xf3ff,
0xf7ff, 0x0700, 0x0b00, 0x0000, 0x0f00, 0x1b00, 0x1700, 0x1600, 0x2100, 0x2600, 0x2700, 0x2e00, 0x2600, 0x2300, 0x2000, 0x1800,
0x0e00, 0x0b00, 0x0200, 0x0300, 0xf3ff, 0xe2ff, 0xe0ff, 0xe0ff, 0xe1ff, 0xdeff, 0xe3ff, 0xe4ff, 0xf2ff, 0xf1ff, 0xfcff, 0xf9ff,
0xf8ff, 0x0000, 0x0300, 0x0900, 0x0a00, 0x0e00, 0x0300, 0x0600, 0x0400, 0x0400, 0x0100, 0x0200, 0x0000, 0x0000, 0xffff, 0xfcff,
0x0100, 0x0100, 0x0300, 0x0400, 0x0800, 0x0500, 0x0500, 0x0000, 0xfaff, 0xffff, 0x0100, 0xfcff, 0xfeff, 0x0000, 0xfcff, 0xf6ff,
0xf5ff, 0xf3ff, 0xf3ff, 0xf0ff, 0xf7ff, 0xfcff, 0x0100, 0x0e00, 0x1800, 0x1900, 0x1d00, 0x2800, 0x2400, 0x2000, 0x2400, 0x2000,
0x1400, 0x0d00, 0x0000, 0xf9ff, 0xeeff, 0xe7ff, 0xe6ff, 0xe3ff, 0xe0ff, 0xdbff, 0xe6ff, 0xeeff, 0xf4ff, 0xfdff, 0xfeff, 0xffff,
0x0000, 0x0400, 0x0000, 0x0500, 0x0500, 0x0900, 0x0500, 0x0600, 0x0c00, 0x0d00, 0x1200, 0x0c00, 0x0d00, 0x0a00, 0x0d00, 0x0b00,
0x0500, 0x0100, 0x0100, 0xfeff, 0xfdff, 0xfaff, 0xfaff, 0xfaff, 0xfbff, 0xf9ff, 0xf8ff, 0xf8ff, 0xf7ff, 0xf8ff, 0xf6ff, 0xf3ff,
0xf8ff, 0xf6ff, 0xf4ff, 0xf2ff, 0xf1ff, 0xefff, 0xf5ff, 0xf3ff, 0xf6ff, 0xfcff, 0xfdff, 0xfcff, 0xffff, 0x0100, 0x0000, 0x0000,
0x0400, 0x1000, 0x1000, 0x1300, 0x1700, 0x1e00, 0x1400, 0x1200, 0x1400, 0x0e00, 0x1400, 0x1100, 0x0600, 0xfbff, 0xf3ff, 0xe9ff,
0xebff, 0xe8ff, 0xe2ff, 0xe7ff, 0xebff, 0xf1ff, 0xf5ff, 0xfeff, 0x0100, 0xffff, 0xfaff, 0xfdff, 0xfeff, 0xfeff, 0xfeff, 0xffff,
0x0900, 0x0c00, 0x1100, 0x1400, 0x1100, 0x1200, 0x0d00, 0x0b00, 0x0f00, 0x0f00, 0x0a00, 0x0600, 0xfeff, 0x0000, 0xfeff, 0xf9ff,
0xfcff, 0xfaff, 0xf8ff, 0xfbff, 0xfeff, 0xf9ff, 0xfcff, 0xf7ff, 0xfbff, 0xf5ff, 0xfcff, 0x0900, 0x0000, 0x0a00, 0x0600, 0xfeff,
0xfbff, 0x1700, 0x1000, 0x0800, 0x1100, 0x0800, 0xf2ff, 0x0400, 0x0300, 0x0000, 0x1d00, 0x1b00, 0x0b00, 0xf1ff, 0xd8ff, 0xddff,
0xeeff, 0xdfff, 0xd0ff, 0xf0ff, 0xf3ff, 0xf4ff, 0x0300, 0x1000, 0xf9ff, 0xf3ff, 0x1000, 0x1e00, 0x0e00, 0xfdff, 0xfaff, 0xfdff,
0x1100, 0xfeff, 0x1d00, 0x3100, 0x0300, 0xf8ff, 0x0c00, 0x1700, 0x1300, 0x0200, 0xfdff, 0x0900, 0x0800, 0xf8ff, 0xf9ff, 0xfaff,
0xf3ff, 0x0400, 0xfcff, 0xf5ff, 0x0100, 0xfaff, 0xe3ff, 0xe4ff, 0xf8ff, 0x0000, 0x0300, 0xf1ff, 0xe1ff, 0x0000, 0xf7ff, 0xf4ff,
0xefff, 0xdfff, 0x0900, 0x1600, 0xe0ff, 0xf5ff, 0x1500, 0x0900, 0xfdff, 0x0300, 0x1200, 0x1900, 0xffff, 0x0e00, 0x3e00, 0x3400,
0xe3ff, 0xfbff, 0x2c00, 0xfdff, 0xe5ff, 0x0300, 0x0800, 0x0200, 0xe5ff, 0xe0ff, 0x0900, 0x1500, 0xdeff, 0xe1ff, 0xf8ff, 0xe0ff,
0xddff, 0x1500, 0x1f00, 0xecff, 0x0c00, 0x6aff, 0x0700, 0x2600, 0xecfe, 0xac01, 0x8500, 0xa9fe, 0xa200, 0x6700, 0xb8ff, 0x8f00,
0xf9ff, 0xa1ff, 0xd100, 0xc1fe, 0x8900, 0x4a01, 0x6dfe, 0xf9ff, 0x1301, 0xb1ff, 0xbaff, 0xac00, 0x1401, 0x7bfe, 0x16ff, 0x8e01,
0x92ff, 0x03ff, 0x7202, 0xedff, 0x58fe, 0x6b00, 0x0300, 0xc6ff, 0x5600, 0xadff, 0xd5ff, 0xe800, 0x85ff, 0x16ff, 0xd000, 0x2600,
0xacff, 0x3f00, 0x47fe, 0x6000, 0x3b02, 0xc1fe, 0x56fe, 0x3001, 0xc2ff, 0xfffe, 0xf4ff, 0x2400, 0x5000, 0x1f00, 0x38ff, 0x0200,
0x6800, 0xc800, 0x45ff, 0xb1ff, 0xeb00, 0x0100, 0xaaff, 0x0f00, 0x7f00, 0x2400, 0x8d00, 0xa5ff, 0x0600, 0xa501, 0xa5ff, 0x67ff,
0xb700, 0x0e00, 0xa6ff, 0xe1ff, 0x0c00, 0xf4ff, 0xf1ff, 0x29ff, 0x0cff, 0xb1ff, 0x2fff, 0xfffe, 0xe0ff, 0x1900, 0xe1ff, 0x1600,
0x5b00, 0x6200, 0x7d00, 0x1f00, 0x1300, 0x6100, 0x4500, 0xf3ff, 0xb400, 0x7800, 0x5d00, 0x1600, 0x4500, 0x4700, 0x1200, 0x0a00,
0xd5ff, 0x5a00, 0x3400, 0xf4ff, 0x3600, 0xbeff, 0xe1ff, 0x0300, 0xb6ff, 0xa5ff, 0xc2ff, 0x98ff, 0xc5ff, 0x2100, 0xdbff, 0x2d00,
0x1400, 0xd4ff, 0x2e00, 0x0a00, 0xfaff, 0x0a00, 0x0300, 0xd9ff, 0xc3ff, 0x6600, 0x1d00, 0x80ff, 0x0700, 0x2300, 0x0eff, 0xacff,
0x0500, 0xacff, 0x1400, 0x4600, 0x5000, 0x6400, 0xad00, 0x4900, 0x2c00, 0x7900, 0x3000, 0xc7ff, 0xbbff, 0x8cff, 0xa3ff, 0x5800,
0xcfff, 0xddfe, 0x8fff, 0xc3ff, 0x78fe, 0x51ff, 0xceff, 0x62ff, 0x2b00, 0xbd00, 0x7300, 0xe000, 0x7901, 0x8900, 0x6e00, 0xf000,
0x5100, 0xd5ff, 0xcfff, 0xacff, 0xe1ff, 0xc400, 0x71ff, 0xbffd, 0xacff, 0xfdff, 0x5dfe, 0x43ff, 0xa000, 0x9b00, 0xab00, 0x1f01,
0x0b01, 0x7e01, 0x1f01, 0xddff, 0xfcff, 0x3200, 0x3fff, 0x16ff, 0x1500, 0xc7ff, 0xddfe, 0x64ff, 0xe6fe, 0x4efe, 0x26ff, 0x4dff,
0x30ff, 0x2800, 0x2401, 0xe300, 0x6301, 0xdf01, 0xa801, 0xb001, 0x5701, 0xc500, 0x8100, 0x3600, 0xa7ff, 0xd6ff, 0x6100, 0x0eff,
0x82fe, 0x9eff, 0xa8ff, 0x03ff, 0x97ff, 0x7300, 0x8200, 0x6600, 0x6000, 0x6700, 0x7c00, 0xdeff, 0x2fff, 0x57ff, 0x83ff, 0x26ff,
0xf3fe, 0x37ff, 0x49ff, 0xe6fe, 0x04ff, 0x66ff, 0x8dff, 0xa6ff, 0x0100, 0x8300, 0xbc00, 0xf200, 0x2501, 0x6301, 0x6601, 0x2b01,
0xed00, 0xbe00, 0x6400, 0x2400, 0x2f00, 0x0d00, 0xe0ff, 0xe5ff, 0xbeff, 0xc4ff, 0x0e00, 0x0600, 0xebff, 0xfbff, 0xd1ff, 0xa5ff,
0x7eff, 0x48ff, 0x2dff, 0x35ff, 0x19ff, 0x62ff, 0x30ff, 0x40ff, 0xbfff, 0x6fff, 0x7eff, 0x0600, 0xe4ff, 0xbaff, 0x4000, 0xae00,
0x8f00, 0x8500, 0xab00, 0xd500, 0xa900, 0x5800, 0x4d00, 0x4a00, 0x1f00, 0xe3ff, 0xd5ff, 0x0500, 0x0e00, 0x5eff, 0xbfff, 0x9500 };
