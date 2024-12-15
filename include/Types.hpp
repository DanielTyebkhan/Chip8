#pragma once
// integer promotion means that there is no reason to store bytes as smaller
// than int since we aren't memory constrained -- unless this causes cache
// misses? --> TODO: profile
using Byte = int;