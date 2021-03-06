#include "APPositionManager.h"
#include "APObjectFactory.h"
#include "APGlobalConfig.h"
#include "APPositionCtrl.h"
#include "Utils/APJsonReader.h"

APPositionManager::APPositionManager()
{
	m_idAccumulator = new APIntAccumulator();
}


APPositionManager::~APPositionManager()
{
	delete m_idAccumulator;
}

APPositionCtrl * APPositionManager::createPositionCtrl(std::string positionInfo)
{
	APJsonReader jr;
	jr.initWithString(positionInfo);
	std::string pcType = "";
	if (jr.hasMember("Type")) {
		pcType = jr.getStrValue("Type");
	}

	APPositionCtrl* pc = APObjectFactory::newPositionCtrl(APGlobalConfig::getInstance()->getInstrumentType(), pcType);
	pc->setID(m_idAccumulator->generateID());
	pc->init(positionInfo);
	m_positionCtrls[pc->getID()] = pc;
	//APTradeManager::getInstance()->registerPositionCtrl(pc->getInstrumentID(), pc->getTradeDirection(), pc);
	return pc;
}

APPositionCtrl * APPositionManager::getPositionCtrl(UINT id) {
	if (m_positionCtrls.find(id) != m_positionCtrls.end()) {
		return m_positionCtrls[id];
	}

	return NULL;
}

void APPositionManager::removePositionCtrl(UINT id) {
	if (m_positionCtrls.find(id) != m_positionCtrls.end()) {
		m_positionCtrls.erase(id);
	}
}

void APPositionManager::removePositionCtrl(APPositionCtrl* pc) {
	if (pc != NULL) {
		removePositionCtrl(pc->getID());
	}
}

void APPositionManager::update()
{
	std::map<UINT, APPositionCtrl*>::iterator it;
	for (it = m_positionCtrls.begin(); it != m_positionCtrls.end(); it++) {
		APPositionCtrl* pc = it->second;
		if (pc != NULL) {
			pc->update();
		}
	}
}

std::map<UINT, APPositionCtrl*>& APPositionManager::getAllPositionCtrls()
{
	return m_positionCtrls;
}

void APPositionManager::onEndTransactionDay()
{
	std::map<UINT, APPositionCtrl*>::iterator it;
	for (it = m_positionCtrls.begin(); it != m_positionCtrls.end(); it++) {
		APPositionCtrl* pc = it->second;
		if (pc != NULL) {
			pc->forceClearOrdersPosition();
			pc->removeAllLocalOrders();
		}
	}
}

void APPositionManager::onStartTransactionDay()
{
	std::map<UINT, APPositionCtrl*>::iterator it;
	for (it = m_positionCtrls.begin(); it != m_positionCtrls.end(); it++) {
		APPositionCtrl* pc = it->second;
		if (pc != NULL) {
			pc->onNewTransactionDay();
		}
	}
}

