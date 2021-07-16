#unecessary
class Path:
    def __init__(self, path_order, slippage):
        self.path_order = path_order
        self.slippage_loss = slippage

#helps with readability
class Order:
    def __init__(self, amount, source, target):
        self.amount = amount
        self.source = source
        self.target = target

#unecessary
class Orderbook:
    def __init__(self, order_book):
        self.order_book = order_book
        self.total_slippage = 0

    def setTotalSlippage(self, total_slippage):
        self.total_slippage = total_slippage
        