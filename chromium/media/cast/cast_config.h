// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_CAST_CONFIG_H_
#define MEDIA_CAST_CAST_CONFIG_H_

#include <stddef.h>
#include <stdint.h>

#include <string>

#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "base/memory/shared_memory.h"
#include "base/single_thread_task_runner.h"
#include "base/time/time.h"
#include "media/cast/net/cast_transport_config.h"

namespace media {
class VideoEncodeAccelerator;

namespace cast {

// TODO(miu): Eliminate these after moving "default config" into the top-level
// media/cast directory.  http://crbug.com/530839
enum SuggestedDefaults {
  // Audio encoder bitrate.  Zero means "auto," which asks the encoder to select
  // a bitrate that dynamically adjusts to the content.  Otherwise, a constant
  // bitrate is used.
  kDefaultAudioEncoderBitrate = 0,

  // Suggested default audio sampling rate.
  kDefaultAudioSamplingRate = 48000,

  // Suggested default maximum video frame rate.
  kDefaultMaxFrameRate = 30,

  // End-to-end latency in milliseconds.
  //
  // DO NOT USE THIS (400 ms is proven as ideal for general-purpose use).
  //
  // TODO(miu): Change to 400, and confirm nothing has broken in later change.
  // http://crbug.com/530839
  kDefaultRtpMaxDelayMs = 100,

  // RTP payload types that identify an RTP stream as audio or video.
  kDefaultRtpAudioPayloadType = 127,
  kDefaultRtpVideoPayloadType = 96,

  // Suggested minimum and maximum video bitrates for general-purpose use (up to
  // 1080p, 30 FPS).
  kDefaultMinVideoKbps = 300,
  kDefaultMaxVideoKbps = 5000,
};

// TODO(miu): Merge AudioSenderConfig and VideoSenderConfig and make their
// naming/documentation consistent with FrameReceiverConfig.
// http://crbug.com/530839
struct AudioSenderConfig {
  AudioSenderConfig();
  ~AudioSenderConfig();

  // Identifier referring to the sender, used by the receiver.
  uint32_t ssrc;

  // The receiver's SSRC identifier.
  uint32_t receiver_ssrc;

  // The total amount of time between a frame's capture/recording on the sender
  // and its playback on the receiver (i.e., shown to a user).  This should be
  // set to a value large enough to give the system sufficient time to encode,
  // transmit/retransmit, receive, decode, and render; given its run-time
  // environment (sender/receiver hardware performance, network conditions,
  // etc.).
  base::TimeDelta min_playout_delay;
  base::TimeDelta max_playout_delay;

  // Starting playout delay when streaming animated content.
  base::TimeDelta animated_playout_delay;

  // RTP payload type enum: Specifies the type/encoding of frame data.
  int rtp_payload_type;

  bool use_external_encoder;
  int frequency;
  int channels;
  int bitrate;  // Set to <= 0 for "auto variable bitrate" (libopus knows best).
  Codec codec;

  // The AES crypto key and initialization vector.  Each of these strings
  // contains the data in binary form, of size kAesKeySize.  If they are empty
  // strings, crypto is not being used.
  std::string aes_key;
  std::string aes_iv_mask;
};

struct VideoSenderConfig {
  VideoSenderConfig();
  ~VideoSenderConfig();

  // Identifier referring to the sender, used by the receiver.
  uint32_t ssrc;

  // The receiver's SSRC identifier.
  uint32_t receiver_ssrc;

  // The total amount of time between a frame's capture/recording on the sender
  // and its playback on the receiver (i.e., shown to a user).  This should be
  // set to a value large enough to give the system sufficient time to encode,
  // transmit/retransmit, receive, decode, and render; given its run-time
  // environment (sender/receiver hardware performance, network conditions,
  // etc.).
  base::TimeDelta min_playout_delay;
  base::TimeDelta max_playout_delay;

  // Starting playout delay when streaming animated content.
  base::TimeDelta animated_playout_delay;

  // RTP payload type enum: Specifies the type/encoding of frame data.
  int rtp_payload_type;

  bool use_external_encoder;

  float congestion_control_back_off;
  int max_bitrate;
  int min_bitrate;
  int start_bitrate;
  int max_qp;
  int min_qp;

  // The maximum |min_quantizer| set to the encoder when CPU is constrained.
  // This is a trade-off between higher resolution with lower encoding quality
  // and lower resolution with higher encoding quality. The set value indicates
  // the maximum quantizer that the encoder might produce better quality video
  // at this resolution than lowering resolution with similar CPU usage and
  // smaller quantizer. The set value has to be between |min_qp| and |max_qp|.
  // Suggested value range: [4, 30].
  int max_cpu_saver_qp;

  int max_frame_rate;  // TODO(miu): Should be double, not int.

  // This field is used differently by various encoders. It defaults to 1.
  //
  // For VP8, this field is ignored.
  //
  // For H.264 on Mac or iOS, it controls the max number of frames the encoder
  // may hold before emitting a frame. A larger window may allow higher encoding
  // efficiency at the cost of latency and memory. Set to 0 to let the encoder
  // choose a suitable value for the platform and other encoding settings.
  int max_number_of_video_buffers_used;

  Codec codec;
  int number_of_encode_threads;

  // The AES crypto key and initialization vector.  Each of these strings
  // contains the data in binary form, of size kAesKeySize.  If they are empty
  // strings, crypto is not being used.
  std::string aes_key;
  std::string aes_iv_mask;
};

// TODO(miu): Naming and minor type changes are badly needed in a later CL.
struct FrameReceiverConfig {
  FrameReceiverConfig();
  ~FrameReceiverConfig();

  // The receiver's SSRC identifier.
  uint32_t receiver_ssrc;

  // The sender's SSRC identifier.
  uint32_t sender_ssrc;

  // The total amount of time between a frame's capture/recording on the sender
  // and its playback on the receiver (i.e., shown to a user).  This is fixed as
  // a value large enough to give the system sufficient time to encode,
  // transmit/retransmit, receive, decode, and render; given its run-time
  // environment (sender/receiver hardware performance, network conditions,
  // etc.).
  int rtp_max_delay_ms;  // TODO(miu): Change to TimeDelta target_playout_delay.

  // RTP payload type enum: Specifies the type/encoding of frame data.
  int rtp_payload_type;

  // RTP timebase: The number of RTP units advanced per one second.  For audio,
  // this is the sampling rate.  For video, by convention, this is 90 kHz.
  int rtp_timebase;

  // Number of channels.  For audio, this is normally 2.  For video, this must
  // be 1 as Cast does not have support for stereoscopic video.
  int channels;

  // The target frame rate.  For audio, this is normally 100 (i.e., frames have
  // a duration of 10ms each).  For video, this is normally 30, but any frame
  // rate is supported.
  int target_frame_rate;

  // Codec used for the compression of signal data.
  // TODO(miu): Merge the AudioCodec and VideoCodec enums into one so this union
  // is not necessary.
  Codec codec;

  // The AES crypto key and initialization vector.  Each of these strings
  // contains the data in binary form, of size kAesKeySize.  If they are empty
  // strings, crypto is not being used.
  std::string aes_key;
  std::string aes_iv_mask;
};

// TODO(miu): Remove the CreateVEA callbacks.  http://crbug.com/454029
typedef base::Callback<void(scoped_refptr<base::SingleThreadTaskRunner>,
                            scoped_ptr<media::VideoEncodeAccelerator>)>
    ReceiveVideoEncodeAcceleratorCallback;
typedef base::Callback<void(const ReceiveVideoEncodeAcceleratorCallback&)>
    CreateVideoEncodeAcceleratorCallback;
typedef base::Callback<void(scoped_ptr<base::SharedMemory>)>
    ReceiveVideoEncodeMemoryCallback;
typedef base::Callback<void(size_t size,
                            const ReceiveVideoEncodeMemoryCallback&)>
    CreateVideoEncodeMemoryCallback;

}  // namespace cast
}  // namespace media

#endif  // MEDIA_CAST_CAST_CONFIG_H_
