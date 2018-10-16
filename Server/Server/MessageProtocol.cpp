

//Protocol Example :
//Header
//int int
//[packet_length][message_id]
//Send message
//? int string int string
//[header][length]][room_name][length][message]
//Receive message
//? int string int string int string
//[header][length][name][length][message][length][room_name]
//Join Room :
//? int string
//[header][length][room_name]
//Leave Room :
//? int string
//[header][length][room_name]