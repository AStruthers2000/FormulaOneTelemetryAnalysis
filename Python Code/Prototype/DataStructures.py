from typing import NamedTuple

class HeaderData(NamedTuple):
    packetFormat: int
    gameMajorVersion: int
    gameMinorVersion: int
    packetVersion: int
    packetID: int
    sessionUID: int
    sessionTime: float
    frameIdentifier: int
    playerCarIndex: int
    secondaryCarIndex: int

class LapData(NamedTuple):
    lastlapTimeMS: int
    currentLapTimeMS: int
    sector1TimeMS: int
    sector2TimeMS: int
    lapDistance: float
    totalDistance: float
    safetyCarDelta: float
    carPosition: int
    currentLapNum: int
    pitStatus: int
    numPitStops: int
    sector: int
    currentLapInvalid: int
    penalties: int
    warnings: int
    numUndervedDriveThroughPens: int
    numUndervedStopGoPens: int
    gridPosition: int
    driverStatus: int #0 = in garage, 1 = flying lap, 2 = in lap, 3 = out lap, 4 = on track
    resultStatus: int #0 = invalid, 1 = inactive, 2 = active, 3 = finished, 4 = dnf, 5 = disq, 6 = not classified, 7 = retired
    pitLaneTimerActive: int
    pitLanetimeInLaneMS: int
    pitStopTimerMS: int
    pitStopShouldServePen: int

class PacketLapData(NamedTuple):
    header: HeaderData
    lapData: LapData

class CarTelemetryData(NamedTuple):
    speed: int
    throttle: float
    steer: float
    brake: float
    clutch: int
    gear: int
    engineRPM: int
    drs: int
    revLightsPercent: int
    revLightsBitValue: int
    brakesTemp: int
    tyresSurfaceTemp: int
    tyresInnerTemp: int
    engineTemp: int
    tyresPressure: float
    surfaceType: int

class PacketCarTelemetryData(NamedTuple):
    header: HeaderData
    carTelemetryData: CarTelemetryData
    mfdPanelIndex: int
    mfdPanelIndexSecondaryPlayer: int
    suggestedGear: int

class Data:
    def __init__(self):
        self.speed: int = 0
        self.drs: int = 0
        self.throttle: float = 0
        self.brake: float = 0
        self.steer: float = 0
        self.lap: int = 0
        self.sector: int = 0
        self.currentLapTime: str = 0
        self.currentLapTimeMS: int = 0
        self.sessionTime: float = 0.0
        self.readyUpdate: int = 0
        self.trackID: int = -1
        self.trackName: str = ""
