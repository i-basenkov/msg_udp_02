#ifndef MSG_TYPES_H
#define MSG_TYPES_H

#include <vector>
#include <queue>
#include <map>

#include "../lib_msg/msgthreads.h"


namespace msg::file_send
{
	namespace pack_type
	{
		constexpr uint8_t ack = 0;
		constexpr uint8_t put = 1;
	}

	using file_t = std::map<std::uint32_t, byte_array_t>;
	using file_list_t = std::map<std::uint64_t, file_t>;
	using file_queue_t = std::queue<file_t>;

	struct sended_seq_t
	{
		uint8_t secs;
		byte_array_t seq;
	};
	using sended_seq_list_t = std::map<std::uint32_t, sended_seq_t>;

	namespace net
	{

		#pragma pack(push)
		#pragma pack(1)
		struct msg_head_pack_t
		{
			std::uint32_t seq_number;
			std::uint32_t seq_total;
			std::uint8_t type;
			std::byte id[8];
		};
		#pragma pack(pop)

		struct msg_head_t{
			std::uint32_t seq_number;
			std::uint32_t seq_total;
			std::uint8_t type;
			std::uint64_t id;
		};

		struct msg_udp
		{
			msg_head_t head;
			byte_array_t data;
		};

		using msg_udp_ts = message_variants_t
		<
			msg_udp
		>;

		using msg_err = message_variants_t
		<
			  msg_error_t
		>;

		using udp_interface_t = thread_interface_t<msg_udp_ts, msg_err, udp_thr>;

	}

    using srv_send_interface_t = thread_interface_t<net::msg_udp_ts>;
	using srv_work_iterface_t = thread_interface_t<net::msg_udp_ts>;

	enum struct msgs_client
	{
		timeout,
		start_send
	};
	using timeout_t = std::integral_constant<msgs_client, msgs_client::timeout>;
	using start_send_t = std::integral_constant<msgs_client, msgs_client::start_send>;

	using client_msg_err = message_variants_t
	<
		  msg_error_t
		, msg_timeout_t
	>;

	using msg_client_ts = message_variants_t
	<
		  net::msg_udp
		, timeout_t
		, start_send_t
	>;


	using client_udp_interface_t = thread_interface_t<net::msg_udp_ts, client_msg_err, udp_thr, timeout<1>>;
    using client_send_interface_t = thread_interface_t<net::msg_udp_ts>;
	using client_work_iterface_t = thread_interface_t<msg_client_ts>;


	struct udp_test
	{
		static
		byte_array_t serializer(net::msg_udp const& m)
		{
			byte_array_t ba;
			net::msg_head_pack_t hd;
			hd.seq_number = byte_swap<endianness::host, endianness::network>(m.head.seq_number);
			hd.seq_total = byte_swap<endianness::host, endianness::network>(m.head.seq_total);
			hd.type = m.head.type;
			memcpy(&hd.id, &m.head.id, 8);
			ba.resize(sizeof(net::msg_head_pack_t));
			memcpy(ba.data(), &hd, sizeof(hd));
			std::move(m.data.begin(), m.data.end(), std::back_inserter(ba));
			return ba;
		}

		static
		net::msg_udp_ts deserializer(byte_array_t& _ba)
		{
			net::msg_udp msg{};
			net::msg_head_pack_t head{};
			memcpy(&head, _ba.data(), sizeof(head));
			msg.head.seq_number = byte_swap<endianness::network, endianness::host>(head.seq_number);
			msg.head.seq_total = byte_swap<endianness::network, endianness::host>(head.seq_total);
			msg.head.type = head.type;
			memcpy(&msg.head.id, &head.id, 8);
			std::move(_ba.begin()+=sizeof(head), _ba.end(), std::back_inserter(msg.data));
			return msg;
		}
	};

}


#endif
