#pragma once

//shared input limits for the console and GUI, db/schema.sql enforces the same ranges
inline constexpr int kMinPoints = 0;
inline constexpr int kMaxPoints = 1000;
inline constexpr int kMinAttempts = 1;
inline constexpr int kMaxAttempts = 100;
