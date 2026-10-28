import asyncio

from websockets.asyncio.client import connect
from websockets.exceptions import ConnectionClosed

SERVER_URI = "ws://127.0.0.1:8013"

async def receive_messages(websocket) -> None:
    """Print messages received from the server until it disconnects."""
    try:
        async for message in websocket:
            print(f"\nReceived: {message}")
            print("Message: ", end="", flush=True)
    except ConnectionClosed:
        print("\nServer closed the connection.")

async def send_messages(websocket) -> None:
    """Read messages from the terminal and send them to the server."""
    print("Type a message and press Enter. Type 'quit' to disconnect.")

    while True:
        message = await asyncio.to_thread(input, "Message: ")

        if message.strip().lower() == "quit":
            return
        if message:
            await websocket.send(message)

async def run_client(uri: str) -> None:
    """Connect to a server and run the sender and receiver together."""
    async with connect(uri) as websocket:
        sender = asyncio.create_task(send_messages(websocket))
        receiver = asyncio.create_task(receive_messages(websocket))

        done, pending = await asyncio.wait(
            (sender, receiver),
            return_when=asyncio.FIRST_COMPLETED,
        )

        for task in pending:
            task.cancel()
        await asyncio.gather(*pending, return_exceptions=True)

        for task in done:
            task.result()

if __name__ == "__main__":
    try:
        asyncio.run(run_client(SERVER_URI))
    except ConnectionRefusedError as e:
        print(f"{e}.\nIs the server online? Correct IP? Correct port?")
    except KeyboardInterrupt:
        print("\nDisconnected.")
