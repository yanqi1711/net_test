import time

def get_bytes(iface):
    with open('/proc/net/dev', 'r') as f:
        for line in f.readlines():
            if line.lstrip().startswith(iface + ':'):
                data = line.split()[1:]
                rx_bytes, tx_bytes = (int(data[0]), int(data[8]))
                return (rx_bytes, tx_bytes)

def get_bw(iface, interval=1):
    rx0, tx0 = get_bytes(iface)
    time.sleep(interval)
    rx1, tx1 = get_bytes(iface)
    rx_bw = (rx1 - rx0) / interval
    tx_bw = (tx1 - tx0) / interval
    return (rx_bw, tx_bw)

if __name__ == '__main__':
    while True:
        rx_bw, tx_bw = get_bw('eth0')
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        print("{} - RX: {:.2f} Mbps\tTX: {:.2f} Mbps".format(timestamp, rx_bw / 1024 / 1024 * 8, tx_bw / 1024 / 1024 * 8))

