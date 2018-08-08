package viabtc

import (
	"testing"

	"math/rand"
	"time"

	"sync"

	"github.com/pkg/errors"
	"github.com/bitlum/viabtc_rpc_client"
)

func asyncOrderCancel(engine *viabtc.Client, orders []viabtc.OrderDetailedInfo) {
	var wg sync.WaitGroup

	limitChan := make(chan struct{}, 50)
	for i := 0; i < 50; i++ {
		limitChan <- struct{}{}
	}

	for _, order := range orders {
		<-limitChan
		wg.Add(1)
		go func(order viabtc.OrderDetailedInfo) {
			defer func() {
				wg.Done()
				limitChan <- struct{}{}
			}()

			req := &viabtc.OrderCancelRequest{
				UserID:  order.UserID,
				Market:  order.Market.String(),
				OrderID: order.OrderID,
			}

			if _, err := engine.OrderCancel(req); err != nil {
				return
			}
		}(order)
	}

	wg.Wait()
}

// getEmptyProfile is a helper function which search user which haven't been
// used in the system yet.
func getEmptyProfile(engine *viabtc.Client) (uint32, error) {
	var userID uint32
	for {
		userID = rand.Uint32()
		req := &viabtc.BalanceQueryRequest{
			UserID: userID,
		}

		if _, err := engine.BalanceQuery(req); err != nil {
			if err, ok := err.(*viabtc.Error); ok {
				continue
			} else {
				return 0, errors.Errorf("unable to get user balance: %s",
					err)
			}
		}

		break
	}

	return userID, nil
}

// cancelOrders is a helper function which is used during test environment
// setup which cancels all orders on the exchange.
func cancelOrders(engine *viabtc.Client) error {
	cancelOrders := func(market viabtc.MarketType, side viabtc.MarketOrderSide) error {
		offset := int32(0)
		var orders []viabtc.OrderDetailedInfo

		for {
			resp, err := engine.OrderBook(&viabtc.OrderBookRequest{
				Market: market.String(),
				Side:   side,
				Offset: offset,
				Limit:  viabtc.MaxLimit,
			})
			if err != nil {
				return err
			}

			orders = append(orders, resp.Orders...)

			l := int32(len(resp.Orders))
			offset += l
			if l < viabtc.MaxLimit {
				break
			}
		}

		asyncOrderCancel(engine, orders)
		return nil
	}

	for _, market := range viabtc.AllMarkets {
		cancelOrders(market, viabtc.MarketOrderSideAsk)
		cancelOrders(market, viabtc.MarketOrderSideBid)
	}

	return nil
}

// setUp is a helper function which is used to setup integration test
// environment. With this function we get two users which not exist in system
// yet, thereby using clear environment. Also in order to not interfere with
// other users activity we have to cancel all orders.
func setUp(engine *viabtc.Client) (*testContext, error) {
	var err error

	firstUserID, err := getEmptyProfile(engine)
	if err != nil {
		return nil, err
	}

	secondUserID, err := getEmptyProfile(engine)
	if err != nil {
		return nil, err
	}

	if firstUserID == secondUserID {
		return nil, errors.New("user ids shouldn't be equal")
	}

	if err := cancelOrders(engine); err != nil {
		return nil, errors.Errorf("unable to cancel orders: %v", err)
	}

	return &testContext{
		client:       engine,
		firstUserID:  firstUserID,
		secondUserID: secondUserID,
	}, nil
}

type testContext struct {
	firstUserID  uint32
	secondUserID uint32
	client       *viabtc.Client
}

// testUpdate is an integration test which checks the ability of exchange client
// update user balances.
func testUpdate(t *testing.T, ctx *testContext) {
	{
		req := &viabtc.BalanceUpdateRequest{
			UserID:     ctx.firstUserID,
			Asset:      viabtc.AssetBTC,
			ActionType: viabtc.ActionDeposit,
			ActionID:   0,
			Change:     "10",
			Detail:     make(map[string]interface{}),
		}

		if _, err := ctx.client.BalanceUpdate(req); err != nil {
			t.Fatalf("unable to deposit: %s", err)
		}
	}

	{
		req := &viabtc.BalanceUpdateRequest{
			UserID:     ctx.firstUserID,
			Asset:      viabtc.AssetBTC,
			ActionType: viabtc.ActionWithdrawal,
			ActionID:   0,
			Change:     "-10",
			Detail:     make(map[string]interface{}),
		}

		if _, err := ctx.client.BalanceUpdate(req); err != nil {
			t.Fatalf("unable to withdraw: %s", err)
		}
	}

	{
		req := &viabtc.BalanceQueryRequest{
			UserID: ctx.firstUserID,
		}

		resp, err := ctx.client.BalanceQuery(req)
		if err != nil {
			t.Fatalf("unable to get user balance: %s", err)
		}

		if resp[viabtc.AssetBTC].Available != "0" {
			t.Fatal("balance is wrong")
		}
	}
}

// testPutOrder is an integration test which checks the ability of exchange
// client to put order of limit and market types.
func testPutOrder(t *testing.T, ctx *testContext) {

	market := viabtc.MarketBTCETH

	// Update fist and second user balances in order to be able to put orders.
	{
		req := &viabtc.BalanceUpdateRequest{
			UserID:     ctx.firstUserID,
			Asset:      market.Stock,
			ActionType: viabtc.ActionDeposit,
			ActionID:   0,
			Change:     "1.0",
			Detail:     make(map[string]interface{}),
		}

		if _, err := ctx.client.BalanceUpdate(req); err != nil {
			t.Fatalf("unable to deposit btc: %s", err)
		}
	}
	{
		req := &viabtc.BalanceUpdateRequest{
			UserID:     ctx.secondUserID,
			Asset:      market.Money,
			ActionType: viabtc.ActionDeposit,
			ActionID:   0,
			Change:     "2.0",
			Detail:     make(map[string]interface{}),
		}

		if _, err := ctx.client.BalanceUpdate(req); err != nil {
			t.Fatalf("unable to deposit ltc: %s", err)
		}
	}

	// Put limit order which specifies the exact price for which users want
	// to sell his BTC and its amount.
	{
		req := &viabtc.OrderPutLimitRequest{
			UserID:       ctx.firstUserID,
			Market:       market.String(),
			Side:         viabtc.MarketOrderSideAsk,
			Amount:       "1.0",
			Price:        "2.0",
			TakerFeeRate: "0.1",
			MakerFeeRate: "0.1",
			Source:       "testPutOrder",
		}

		if _, err := ctx.client.OrderPutLimit(req); err != nil {
			t.Fatalf("unable to put order: %s", err)
		}
	}

	// Put market order which wil be executed with current price of the
	// market. In this case users specifies the amount of CNY which he want's
	// to spent on buying the BTC.
	{
		req := &viabtc.OrderPutMarketRequest{
			UserID:       ctx.secondUserID,
			Market:       market.String(),
			Side:         viabtc.MarketOrderSideBid,
			Amount:       "2.0",
			TakerFeeRate: "0.1",
			Source:       "testPutOrder",
		}

		if _, err := ctx.client.OrderPutMarket(req); err != nil {
			t.Fatalf("unable to put order: %s", err)
		}
	}

	// Check that balances of the users have been switched.
	{
		req := &viabtc.BalanceQueryRequest{
			UserID: ctx.firstUserID,
		}

		resp, err := ctx.client.BalanceQuery(req)
		if err != nil {
			t.Fatalf("unable to get first user balance: %s", err)
		}

		if resp[market.Stock].Available != "0" {
			t.Fatalf("first user: wrong btc balance: expected %v, "+
				"available: %v", "0", resp[market.Stock].Available)
		}

		if resp[market.Stock].Freeze != "0" {
			t.Fatalf("first user: wrong btc balance: expected %v, "+
				"freeze: %v", "0", resp[market.Stock].Freeze)
		}

		if resp[market.Money].Available != "1.8" {
			t.Fatalf("first user: wrong cny balance: expected %v, "+
				"available: %v", "1.8", resp[market.Money].Available)
		}

		if resp[market.Money].Freeze != "0" {
			t.Fatalf("first user: wrong cny balance: expected %v, "+
				"freeze: %v", "0", resp[market.Money].Freeze)
		}
	}

	{
		req := &viabtc.BalanceQueryRequest{
			UserID: ctx.secondUserID,
		}

		resp, err := ctx.client.BalanceQuery(req)
		if err != nil {
			t.Fatalf("unable to get second user balance: %s", err)
		}

		if resp[market.Stock].Available != "0.9" {
			t.Fatalf("second user: wrong btc balance: expected %v, "+
				"available: %v", "0.9", resp[market.Stock].Available)
		}

		if resp[market.Stock].Freeze != "0" {
			t.Fatalf("second user: wrong btc balance: expected %v, "+
				"freeze: %v", "0", resp[market.Stock].Freeze)
		}

		if resp[market.Money].Available != "0" {
			t.Fatalf("second user: wrong cny balance: expected %v, "+
				"available: %v", "0", resp[market.Money].Available)
		}

		if resp[market.Money].Freeze != "0" {
			t.Fatalf("second user: wrong cny balance: expected %v, "+
				"freeze: %v", "0", resp[market.Money].Freeze)
		}
	}
}

// testOrderDepth is an integration test which checks the behavior of the
// request depth interval.
func testOrderDepth(t *testing.T, ctx *testContext) {
	// Update fist and second user balances in order to be able to put orders.
	{
		req := &viabtc.BalanceUpdateRequest{
			UserID:     ctx.firstUserID,
			Asset:      viabtc.AssetBTC,
			ActionType: viabtc.ActionDeposit,
			ActionID:   0,
			Change:     "6.0",
			Detail:     make(map[string]interface{}),
		}

		if _, err := ctx.client.BalanceUpdate(req); err != nil {
			t.Fatalf("unable to deposit btc: %s", err)
		}
	}

	// Put limit order which specifies the exact price for which users want
	// to sell his BTC and its amount.
	{
		req := &viabtc.OrderPutLimitRequest{
			UserID:       ctx.firstUserID,
			Market:       viabtc.MarketBTCETH.String(),
			Side:         viabtc.MarketOrderSideBid,
			Amount:       "1.0",
			Price:        "1.0",
			TakerFeeRate: "0.1",
			MakerFeeRate: "0.1",
			Source:       "testOrderDepth",
		}

		if _, err := ctx.client.OrderPutLimit(req); err != nil {
			t.Fatalf("unable to put order: %s", err)
		}
	}
	{
		req := &viabtc.OrderPutLimitRequest{
			UserID:       ctx.firstUserID,
			Market:       viabtc.MarketBTCETH.String(),
			Side:         viabtc.MarketOrderSideBid,
			Amount:       "1.0",
			Price:        "2.0",
			TakerFeeRate: "0.1",
			MakerFeeRate: "0.1",
			Source:       "testOrderDepth",
		}

		if _, err := ctx.client.OrderPutLimit(req); err != nil {
			t.Fatalf("unable to put order: %s", err)
		}
	}
	{
		req := &viabtc.OrderPutLimitRequest{
			UserID:       ctx.firstUserID,
			Market:       viabtc.MarketBTCETH.String(),
			Side:         viabtc.MarketOrderSideBid,
			Amount:       "1.0",
			Price:        "3.0",
			TakerFeeRate: "0.1",
			MakerFeeRate: "0.1",
			Source:       "testOrderDepth",
		}

		if _, err := ctx.client.OrderPutLimit(req); err != nil {
			t.Fatalf("unable to put order: %s", err)
		}
	}

	{
		req := &viabtc.OrderDepthRequest{
			Market:   viabtc.MarketBTCETH.String(),
			Limit:    viabtc.MaxLimit,
			Interval: "1",
		}

		resp, err := ctx.client.OrderDepth(req)
		if err != nil {
			t.Fatalf("unable to fetch orders depth: %s", err)
		}

		if resp.Bids[0].Price != "3" {
			t.Fatal("price is wrong")
		}

		if resp.Bids[0].Volume != "1" {
			t.Fatal("volume is wrong")
		}

		if resp.Bids[1].Price != "2" {
			t.Fatal("price is wrong")
		}

		if resp.Bids[1].Volume != "1" {
			t.Fatal("volume is wrong")
		}

		if resp.Bids[2].Price != "1" {
			t.Fatal("price is wrong")
		}

		if resp.Bids[2].Volume != "1" {
			t.Fatal("volume is wrong")
		}
	}
}

// testStressUpdate is an integration test which checks the thread
// safeness of exchange client.
func testStressUpdate(t *testing.T, ctx *testContext) {
	done := make(chan struct{})
	errChan := make(chan error)

	for i := 0; i < 100; i++ {
		go func(i int) {
			req := &viabtc.BalanceUpdateRequest{
				UserID:     ctx.firstUserID,
				Asset:      viabtc.AssetBTC,
				ActionType: viabtc.ActionDeposit,
				ActionID:   int32(i),
				Change:     "1",
				Detail:     make(map[string]interface{}),
			}

			if _, err := ctx.client.BalanceUpdate(req); err != nil {
				errChan <- errors.Errorf("unable to deposit: %s", err)
			}

			done <- struct{}{}
		}(i)
	}

	for i := 0; i < 100; i++ {
		select {
		case <-done:
		case err := <-errChan:
			t.Fatal(err)
		case <-time.Tick(time.Second * 20):
			t.Fatal("timeout")
		}
	}

	{
		req := &viabtc.BalanceUpdateRequest{
			UserID:     ctx.firstUserID,
			Asset:      viabtc.AssetBTC,
			ActionType: viabtc.ActionWithdrawal,
			ActionID:   0,
			Change:     "-100",
			Detail:     make(map[string]interface{}),
		}

		if _, err := ctx.client.BalanceUpdate(req); err != nil {
			t.Fatalf("unable to withdraw: %s", err)
		}
	}
}

// testOrderPending is an integration test which checks operability of
// pending request.
func testOrderPending(t *testing.T, ctx *testContext) {
	// Update fist and second user balances in order to be able to put orders.
	{
		req := &viabtc.BalanceUpdateRequest{
			UserID:     ctx.firstUserID,
			Asset:      viabtc.AssetBTC,
			ActionType: viabtc.ActionDeposit,
			ActionID:   0,
			Change:     "3.0",
			Detail:     make(map[string]interface{}),
		}

		if _, err := ctx.client.BalanceUpdate(req); err != nil {
			t.Fatalf("unable to deposit btc: %s", err)
		}
	}

	// Put limit order which specifies the exact price for which users want
	// to sell his BTC and its amount.
	{
		req := &viabtc.OrderPutLimitRequest{
			UserID:       ctx.firstUserID,
			Market:       viabtc.MarketBTCLTC.String(),
			Side:         viabtc.MarketOrderSideBid,
			Amount:       "1.0",
			Price:        "1.0",
			TakerFeeRate: "0.1",
			MakerFeeRate: "0.1",
			Source:       "testOrderDepth",
		}

		if _, err := ctx.client.OrderPutLimit(req); err != nil {
			t.Fatalf("unable to put order: %s", err)
		}
	}

	{
		req := &viabtc.OrderPendingRequest{
			UserID: ctx.firstUserID,
			Market: viabtc.MarketBTCLTC.String(),
			Offset: 0,
			Limit:  viabtc.MaxLimit,
		}

		if _, err := ctx.client.OrderPending(req); err != nil {
			t.Fatalf("unable to put order: %s", err)
		}
	}
}

func testKline(t *testing.T, ctx *testContext) {
	{
		req := &viabtc.MarketKLineRequest{
			Market:    viabtc.MarketBTCLTC.String(),
			StartTime: 2 ^ 64 - 1,
			EndTime:   2 ^ 64 - 1,
			Interval:  3,
		}

		if _, err := ctx.client.MarketKLine(req); err != nil {
			t.Fatalf("unable to get kline: %s", err)
		}
	}
}

var testCases = []struct {
	name string
	test func(t *testing.T, ctx *testContext)
}{
	{
		name: "update user balance once",
		test: testUpdate,
	},
	{
		name: "put limit and market orders",
		test: testPutOrder,
	},
	{
		name: "update user balance stress test",
		test: testStressUpdate,
	},
	{
		name: "check market depth",
		test: testOrderDepth,
	},
	{
		name: "check order pending details",
		test: testOrderPending,
	},
	{
		name: "fetch kline",
		test: testKline,
	},
}

// TestApi is an main test which is used to run the integration
// exchange client subtests.
func TestApi(t *testing.T) {
	rand.Seed(time.Now().Unix())

	client := viabtc.NewClient(&viabtc.Config{
		Host: "localhost",
		Port: 8080,
	})

	t.Logf("Running %v integration tests", len(testCases))
	for _, testCase := range testCases {
		success := t.Run(testCase.name, func(t *testing.T) {
			context, err := setUp(client)
			if err != nil {
				t.Fatalf("unable to set up context: %v", err)
			}

			testCase.test(t, context)
			t.Logf("Passed(%v)", testCase.name)
		})

		// Stop at the first failure.
		if !success {
			break
		}
	}
}
