#ifndef OPERATIONS
#define OPERATIONS

#define ALLKEYS 'a'
#define ADD_PEER '0'
#define REMOVE_PEER '1'
#define ADD_CONTENT '2'
#define REMOVE_CONTENT '3'
#define LOOKUP_CONTENT '4'

#define MOVE_CONTENT '5'
#define CHANGE_NUMPEERS_CONTENT '6'
#define CHANGE_NEIGHBOUR '7'

#define NONGOD_LOOKUP_CONTENT 'l'

#define LEFT 0
#define RIGHT 1

#endif




//Define new message types that start with unique characters followed by
//sizeof(sockaddr_in) bytes representing who received the god call
//before passing the message on to the next peer, check if the next peer is
//exactly that sockaddr, if it is, go back in reverse order and say can't be found.

//eg, get and change num peers and content are non-god calls





//TODO:
/*
	add additional peers
		requires setting up left and right peer for new peers
		update num peers (for all peers) on add/remove peers
		update num content (for all peers) on add/remove content


	remove peers
		reditribute dead peer's content
		update neighbors

	setting left/right peers and updating neighbors

	load balancing
		on remove content
			request more items from neighbors (if necessary)
		on adding content
			find where it should go

*/