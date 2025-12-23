import os
import json
import time
import numpy as np
import matplotlib.pyplot as plt
from dynamixel_sdk import *
from enum import Enum

class MultiJointDynamixelController:
    def __init__(self, port="COM3", baudrate=1000000):
        """다이나믹셀 컨트롤러 초기화 (Windows 환경)"""
        self.port = port
        self.baudrate = baudrate
        
        # 다이나믹셀 SDK 핸들러 초기화
        self.portHandler = PortHandler(self.port)
        self.packetHandler = PacketHandler(2.0)  # 프로토콜 2.0 사용
        
        # 다축 읽기를 위한 그룹 싱크리드 설정
        self.groupSyncRead = GroupSyncRead(
            self.portHandler,
            self.packetHandler,
            132,  # ADDR_PRESENT_POSITION
            4     # LEN_PRESENT_POSITION
        )
        
        # ⭐ 다축 쓰기를 위한 그룹 싱크 라이트 설정 추가
        self.groupSyncWrite = GroupSyncWrite(
            self.portHandler,
            self.packetHandler,
            116,  # ADDR_GOAL_POSITION
            4     # LEN_GOAL_POSITION
        )
        
        # 주소 상수
        self.ADDR_OPERATING_MODE = 11
        self.ADDR_TORQUE_ENABLE = 64
        self.ADDR_PROFILE_VELOCITY = 112
        self.ADDR_GOAL_POSITION = 116
        self.ADDR_PRESENT_POSITION = 132
        
        # 연결 설정
        try:
            if not self.portHandler.openPort():
                raise Exception(f"Failed to open port {self.port}")
            
            if not self.portHandler.setBaudRate(self.baudrate):
                raise Exception(f"Failed to set baudrate to {self.baudrate}")
                
            print(f"Dynamixel controller initialized on {self.port} at {self.baudrate} baud")
            
        except Exception as e:
            print(f"Error initializing controller: {e}")
            raise
    
    def setup_motors(self, motor_ids, velocity=1023):
        """여러 모터 초기 설정 (기본적으로 최고 속도)"""
        for motor_id in motor_ids:
            try:
                # 토크 비활성화
                dxl_comm_result, dxl_error = self.packetHandler.write1ByteTxRx(
                    self.portHandler, motor_id, self.ADDR_TORQUE_ENABLE, 0
                )
                if dxl_comm_result != COMM_SUCCESS:
                    print(f"Failed to disable torque on motor {motor_id}: {self.packetHandler.getTxRxResult(dxl_comm_result)}")
                    continue
                
                # 확장 위치 제어 모드 설정 (모드 4로 변경)
                dxl_comm_result, dxl_error = self.packetHandler.write1ByteTxRx(
                    self.portHandler, motor_id, self.ADDR_OPERATING_MODE, 4
                )
                if dxl_comm_result != COMM_SUCCESS:
                    print(f"Failed to set operating mode on motor {motor_id}: {self.packetHandler.getTxRxResult(dxl_comm_result)}")
                    continue
                
                # 프로파일 속도 설정 (최고 속도)
                dxl_comm_result, dxl_error = self.packetHandler.write4ByteTxRx(
                    self.portHandler, motor_id, self.ADDR_PROFILE_VELOCITY, velocity
                )
                if dxl_comm_result != COMM_SUCCESS:
                    print(f"Failed to set velocity on motor {motor_id}: {self.packetHandler.getTxRxResult(dxl_comm_result)}")
                    continue
                
                # 토크 활성화
                dxl_comm_result, dxl_error = self.packetHandler.write1ByteTxRx(
                    self.portHandler, motor_id, self.ADDR_TORQUE_ENABLE, 1
                )
                if dxl_comm_result != COMM_SUCCESS:
                    print(f"Failed to enable torque on motor {motor_id}: {self.packetHandler.getTxRxResult(dxl_comm_result)}")
                    continue
                
                # 싱크 리드 파라미터 추가
                self.groupSyncRead.addParam(motor_id)
                
                print(f"Motor ID {motor_id} setup complete (Extended Position Mode, Velocity: {velocity})")
                
            except Exception as e:
                print(f"Error setting up motor {motor_id}: {e}")
    
    def set_multiple_positions_simultaneously(self, motor_positions):
        """여러 모터 위치를 진짜 동시에 설정 (GroupSyncWrite 사용)"""
        # 기존 데이터 클리어
        self.groupSyncWrite.clearParam()
        
        # 각 모터의 목표 위치 추가
        for motor_id, position in motor_positions.items():
            position = max(-256000, min(256000, int(position)))
            
            # 4바이트 위치 데이터 생성 (리틀 엔디안)
            position_bytes = [
                position & 0xFF,
                (position >> 8) & 0xFF,
                (position >> 16) & 0xFF,
                (position >> 24) & 0xFF
            ]
            
            # 그룹에 추가
            dxl_addparam_result = self.groupSyncWrite.addParam(motor_id, position_bytes)
            if not dxl_addparam_result:
                print(f"Failed to add param for motor {motor_id}")
        
        # 모든 모터에 동시 전송 ⚡
        dxl_comm_result = self.groupSyncWrite.txPacket()
        
        if dxl_comm_result != COMM_SUCCESS:
            print(f"Failed to send group sync write: {self.packetHandler.getTxRxResult(dxl_comm_result)}")
            return False
        
        return True
    
    def set_single_position(self, motor_id, position):
        """단일 모터 위치 설정 (기존 호환용)"""
        position = max(-256000, min(256000, int(position)))
        
        dxl_comm_result, dxl_error = self.packetHandler.write4ByteTxRx(
            self.portHandler, motor_id, self.ADDR_GOAL_POSITION, position
        )
        
        if dxl_comm_result != COMM_SUCCESS:
            print(f"Failed to set position for motor {motor_id}: {self.packetHandler.getTxRxResult(dxl_comm_result)}")
        
        return dxl_comm_result == COMM_SUCCESS
    
    def read_positions(self, motor_ids):
        """여러 모터 위치 동시 읽기 (부호 변환 포함)"""
        # 데이터 요청
        dxl_comm_result = self.groupSyncRead.txRxPacket()
        
        # 결과 저장할 딕셔너리
        positions = {}
        
        if dxl_comm_result != COMM_SUCCESS:
            print(f"Failed to read positions: {self.packetHandler.getTxRxResult(dxl_comm_result)}")
        else:
            # 각 모터에서 데이터 읽기
            for motor_id in motor_ids:
                # 모터에서 데이터를 성공적으로 받았는지 확인
                if self.groupSyncRead.isAvailable(motor_id, self.ADDR_PRESENT_POSITION, 4):
                    # 현재 위치 가져오기
                    position = self.groupSyncRead.getData(motor_id, self.ADDR_PRESENT_POSITION, 4)
                    
                    # 🔧 부호 변환: 32비트 부호 있는 정수로 변환
                    if position > 2147483647:  # 2^31 - 1
                        position = position - 4294967296  # 2^32
                    
                    positions[motor_id] = position
                else:
                    print(f"Failed to get position data from motor ID {motor_id}")
                    positions[motor_id] = None
        
        return positions
    
    def close(self):
        """포트 닫기"""
        self.portHandler.closePort()
        print("Port closed")

class SpeedOverflowStrategy(Enum):
    """속도 초과 시 처리 전략"""
    REJECT = "reject"                    # 재생 거부
    AUTO_SLOW = "auto_slow"             # 자동으로 속도 감소
    FRAME_SKIP = "frame_skip"           # 프레임 스킵
    INTERPOLATE = "interpolate"         # 프레임 보간


class TimeBasedSimultaneousPlayer:
    def __init__(self, controller, animation_file):
        """시간 기반 동시 애니메이션 플레이어 초기화 (상대 모드 전용)"""
        self.controller = controller
        
        # 애니메이션 데이터 로드
        try:
            with open(animation_file, 'r') as f:
                self.animation_data = json.load(f)
            
            # 메타데이터 추출
            self.metadata = self.animation_data["metadata"]
            self.motors = self.animation_data["motors"]
            self.fps = self.metadata["fps"]
            
            # 모터 ID 목록 생성
            self.motor_ids = [self.motors[joint]["motor_id"] for joint in self.motors]
            
            # 모터별 방향 설정 (True = 정방향, False = 역방향)
            self.motor_directions = {}
            for motor_id in self.motor_ids:
                self.motor_directions[motor_id] = True  # 기본값: 정방향
            
            # 속도 분석을 위한 변수들
            self.max_position_change_per_frame = {}
            self.calculate_max_position_changes()
            
            # 상대 모드를 위한 변수들
            self.base_positions = {}  # 각 모터의 기준 위치
            self.animation_offsets = {}  # 애니메이션 오프셋
            
            print(f"Loaded animation with {len(self.motor_ids)} motor joints")
            print(f"Motor IDs: {self.motor_ids}")
            print(f"Duration: {self.metadata['duration_seconds']} seconds")
            print("🚀 상대 위치 동시 제어 모드 - 현재 모터 위치에서 시작합니다")
            
        except Exception as e:
            print(f"Error loading animation file: {e}")
            raise
    
    def calculate_max_position_changes(self):
        """각 모터별 프레임 간 최대 위치 변화량 계산"""
        frames = self.animation_data["frames"]
        
        for motor_id in self.motor_ids:
            max_change = 0
            prev_position = None
            
            for frame in frames:
                for joint_name, joint_data in frame["joints"].items():
                    if joint_data["motor_id"] == motor_id:
                        current_position = joint_data["dynamixel_position"]
                        
                        if prev_position is not None:
                            change = abs(current_position - prev_position)
                            max_change = max(max_change, change)
                        
                        prev_position = current_position
                        break
            
            self.max_position_change_per_frame[motor_id] = max_change
            print(f"Motor ID {motor_id}: Max position change per frame = {max_change} units ({max_change*360/4096:.1f}°)")
    
    def calculate_animation_offsets(self):
        """애니메이션의 상대적 변화량 계산"""
        frames = self.animation_data["frames"]
        
        # 각 모터별 첫 번째 프레임 위치를 기준점으로 설정
        first_frame = frames[0]
        motor_base_values = {}
        
        for joint_name, joint_data in first_frame["joints"].items():
            motor_id = joint_data["motor_id"]
            base_value = joint_data["dynamixel_position"]
            motor_base_values[motor_id] = base_value
        
        # 모든 프레임에 대해 기준점 대비 오프셋 계산
        self.animation_offsets = {}
        
        for motor_id in self.motor_ids:
            self.animation_offsets[motor_id] = []
            base_value = motor_base_values[motor_id]
            
            for frame in frames:
                for joint_name, joint_data in frame["joints"].items():
                    if joint_data["motor_id"] == motor_id:
                        current_value = joint_data["dynamixel_position"]
                        offset = current_value - base_value
                        self.animation_offsets[motor_id].append(offset)
                        break
        
        print("\n=== 🎯 상대 위치 애니메이션 분석 ===")
        
        # 오프셋 범위 분석
        for motor_id in self.motor_ids:
            offsets = self.animation_offsets[motor_id]
            min_offset = min(offsets)
            max_offset = max(offsets)
            range_offset = max_offset - min_offset
            
            print(f"모터 {motor_id}: 오프셋 범위 {min_offset} ~ {max_offset} "
                  f"(총 변화량: {range_offset} units, {range_offset*360/4096:.1f}°)")
    
    def set_base_positions(self):
        """현재 모터 위치들을 기준점으로 설정"""
        print("\n📍 현재 모터 위치를 기준점으로 설정 중...")
        
        current_positions = self.controller.read_positions(self.motor_ids)
        
        for motor_id in self.motor_ids:
            current_pos = current_positions.get(motor_id)
            if current_pos is not None:
                self.base_positions[motor_id] = current_pos
                current_angle = current_pos * 360 / 4096
                print(f"  모터 {motor_id}: 기준 위치 {current_pos} units ({current_angle:.1f}°)")
            else:
                print(f"  ⚠️ 모터 {motor_id}: 위치 읽기 실패, 기본값 0 사용")
                self.base_positions[motor_id] = 0
        
        print("✅ 기준 위치 설정 완료")
    
    def get_relative_position(self, motor_id, frame_index):
        """프레임 인덱스에 해당하는 상대 위치 계산"""
        if motor_id not in self.base_positions or motor_id not in self.animation_offsets:
            return 0
        
        base_pos = self.base_positions[motor_id]
        offset = self.animation_offsets[motor_id][frame_index]
        
        # 방향 설정 적용 (오프셋에만 적용)
        if not self.motor_directions.get(motor_id, True):  # 역방향인 경우
            offset = -offset
        
        absolute_pos = base_pos + offset
        
        # 확장 위치 모드 범위 제한
        absolute_pos = max(-256000, min(256000, int(absolute_pos)))
        
        return absolute_pos
    
    def set_motor_direction(self, motor_id, reverse=False):
        """특정 모터의 회전 방향 설정"""
        if motor_id in self.motor_ids:
            self.motor_directions[motor_id] = not reverse
            direction_str = "역방향" if reverse else "정방향"
            print(f"모터 ID {motor_id}: {direction_str} 설정됨")
        else:
            print(f"모터 ID {motor_id}를 찾을 수 없습니다")
    
    def configure_motor_directions(self):
        """사용자가 각 모터의 방향을 설정할 수 있는 함수"""
        print(f"\n=== 모터 회전 방향 설정 ===")
        print(f"총 {len(self.motor_ids)}개의 모터가 있습니다")
        
        for motor_id in self.motor_ids:
            while True:
                response = input(f"모터 ID {motor_id}의 방향을 반대로 하시겠습니까? (y/n, default: n): ").lower()
                if response in ['y', 'yes']:
                    self.set_motor_direction(motor_id, reverse=True)
                    break
                elif response in ['n', 'no', '']:
                    self.set_motor_direction(motor_id, reverse=False)
                    break
                else:
                    print("y 또는 n을 입력하세요")
        
        print("\n=== 최종 모터 방향 설정 ===")
        for motor_id in self.motor_ids:
            direction_str = "정방향" if self.motor_directions[motor_id] else "역방향"
            print(f"모터 ID {motor_id}: {direction_str}")
    
    def check_speed_feasibility(self, speed_factor=1.0):
        """주어진 속도로 재생 가능한지 사전 체크"""
        frame_time = 1.0 / (self.fps * speed_factor)
        feasible = True
        
        print(f"\n=== Speed Feasibility Check ===")
        print(f"Speed factor: {speed_factor}x (frame time: {frame_time:.3f}s)")
        
        max_feasible_speed = float('inf')
        
        for motor_id in self.motor_ids:
            max_change = self.max_position_change_per_frame[motor_id]
            
            # 필요한 각속도 계산 (간단화)
            angle_change = max_change * 360 / 4096
            required_angular_velocity = angle_change / frame_time
            required_velocity_units = required_angular_velocity * 60 / (360 * 0.229)
            
            if required_velocity_units > 1023:  # 다이나믹셀 최대 속도 초과
                print(f"❌ Motor {motor_id}: Requires {required_velocity_units:.0f} units (MAX: 1023)")
                feasible = False
            else:
                print(f"✓ Motor {motor_id}: Requires {required_velocity_units:.0f} units")
            
            # 이 모터로 가능한 최대 속도 계산
            if max_change > 0:
                motor_max_speed = (1023 * 0.229 * 360) / (60 * max_change * 360 / 4096 * self.fps)
                max_feasible_speed = min(max_feasible_speed, motor_max_speed)
        
        if not feasible:
            if max_feasible_speed != float('inf'):
                print(f"💡 Recommended maximum speed factor: {max_feasible_speed:.2f}x")
        
        return feasible, max_feasible_speed if max_feasible_speed != float('inf') else speed_factor
    
    def handle_speed_overflow(self, speed_factor, strategy):
        """속도 초과 상황 처리"""
        feasible, max_feasible_speed = self.check_speed_feasibility(speed_factor)
        
        if feasible:
            print(f"✓ Speed {speed_factor}x is feasible for all motors")
            return speed_factor, "original"
        
        print(f"❌ Speed {speed_factor}x exceeds motor capabilities")
        
        if strategy == SpeedOverflowStrategy.REJECT:
            print(f"🚫 Rejecting playback at {speed_factor}x speed")
            print(f"💡 Maximum feasible speed: {max_feasible_speed:.2f}x")
            return None, "rejected"
            
        elif strategy == SpeedOverflowStrategy.AUTO_SLOW:
            safe_speed = max_feasible_speed * 0.9
            print(f"🔄 Auto-adjusting speed to {safe_speed:.2f}x (90% of maximum)")
            return safe_speed, "auto_slowed"
            
        elif strategy == SpeedOverflowStrategy.FRAME_SKIP:
            skip_ratio = max(2, int(speed_factor / max_feasible_speed) + 1)
            effective_speed = max_feasible_speed * 0.9
            print(f"⏭️  Frame skip strategy: Playing every {skip_ratio} frames")
            print(f"Effective speed: {effective_speed:.2f}x with {skip_ratio}x frame skip")
            return effective_speed, f"frame_skip_{skip_ratio}"
            
        elif strategy == SpeedOverflowStrategy.INTERPOLATE:
            interpolation_factor = max(2, int(speed_factor / max_feasible_speed) + 1)
            new_speed = max_feasible_speed * 0.9
            print(f"🔄 Interpolation strategy: {interpolation_factor}x frame interpolation")
            print(f"Playing at {new_speed:.2f}x with smoother motion")
            return new_speed, f"interpolated_{interpolation_factor}"
        
        else:
            # 기본값: 자동 속도 감소
            safe_speed = max_feasible_speed * 0.9
            print(f"🔄 Auto-adjusting speed to {safe_speed:.2f}x (90% of maximum)")
            return safe_speed, "auto_slowed"
    
    def setup(self):
        """모터 초기 설정 (항상 최고 속도)"""
        print("Setting up motors with maximum velocity (1023)...")
        self.controller.setup_motors(self.motor_ids, velocity=1023)
        
        # 상대 모드 초기화
        self.calculate_animation_offsets()
        self.set_base_positions()
    
    def play_simultaneous_relative(self, speed_factor=1.0, strategy=SpeedOverflowStrategy.AUTO_SLOW):
        """상대 위치 기반 동시 애니메이션 재생 (GroupSyncWrite 사용)"""
        
        # 속도 초과 처리
        adjusted_speed, strategy_used = self.handle_speed_overflow(speed_factor, strategy)
        
        if adjusted_speed is None:
            print("Playback cancelled due to speed constraints.")
            return None, None, None, None
        
        frames = self.animation_data["frames"]
        
        # 프레임 스킵 처리
        if strategy_used.startswith("frame_skip"):
            skip_ratio = int(strategy_used.split("_")[-1])
            frames = frames[::skip_ratio]
            print(f"Using frame skip: {len(frames)} frames")
        
        # 기록용 데이터
        times = []
        target_positions = {motor_id: [] for motor_id in self.motor_ids}
        actual_positions = {motor_id: [] for motor_id in self.motor_ids}
        position_errors = {motor_id: [] for motor_id in self.motor_ids}
        
        print(f"\n=== 🚀 상대 위치 동시 애니메이션 재생 (GroupSyncWrite) ===")
        print(f"최종 속도: {adjusted_speed}x, 총 프레임: {len(frames)}")
        print(f"모터 속도: 1023 (최고 속도)")
        print("현재 위치 기준 상대 제어 + 모든 모터 진짜 동시 제어")
        
        start_time = time.time()
        
        try:
            for i, frame in enumerate(frames):
                current_time = (time.time() - start_time) * adjusted_speed
                target_time = frame["time"] / adjusted_speed
                
                # 시간 동기화
                if current_time < target_time:
                    time.sleep(target_time - current_time)
                
                # ⭐ 모든 모터의 상대 위치 계산
                motor_positions = {}
                for motor_id in self.motor_ids:
                    final_position = self.get_relative_position(motor_id, i)
                    motor_positions[motor_id] = final_position
                
                # ⚡ 모든 모터에 동시 명령 전송 (GroupSyncWrite)
                success = self.controller.set_multiple_positions_simultaneously(motor_positions)
                
                # 디버깅 출력 (20프레임마다)
                if i % 20 == 0:
                    motor_info = []
                    for motor_id in self.motor_ids:
                        direction_str = "정방향" if self.motor_directions[motor_id] else "역방향"
                        base_pos = self.base_positions[motor_id]
                        offset = self.animation_offsets[motor_id][i]
                        final_pos = motor_positions[motor_id]
                        motor_info.append(f"M{motor_id}({direction_str}): {base_pos}+{offset}={final_pos}")
                    
                    print(f"Frame {i+1}/{len(frames)} | 시간: {target_time:.2f}s | {' | '.join(motor_info)}")
                
                # 위치 설정 후 짧은 대기 (GroupSyncWrite는 더 빠르므로 짧게)
                time.sleep(0.0005)
                
                # 모든 모터 위치 읽기
                actual_position_dict = self.controller.read_positions(self.motor_ids)
                
                # 데이터 기록
                times.append(target_time)
                
                for motor_id in self.motor_ids:
                    target_pos = motor_positions[motor_id]
                    actual_pos = actual_position_dict.get(motor_id, 0)
                    
                    target_positions[motor_id].append(target_pos)
                    actual_positions[motor_id].append(actual_pos)
                    
                    if actual_pos is not None:
                        error = abs(target_pos - actual_pos)
                        position_errors[motor_id].append(error)
                    else:
                        position_errors[motor_id].append(None)
                
                # 진행률 표시
                if i % 30 == 0:
                    progress = (i + 1) / len(frames) * 100
                    print(f"진행률: {progress:.1f}%", end="\r")
            
            print(f"\n\n=== ✅ 상대 위치 동시 애니메이션 완료! ===")
            
            # 최종 위치 요약
            print("\n=== 최종 위치 요약 (상대 모드) ===")
            final_positions = self.controller.read_positions(self.motor_ids)
            
            for motor_id in self.motor_ids:
                base_pos = self.base_positions[motor_id]
                final_pos = final_positions.get(motor_id, 0)
                total_movement = final_pos - base_pos if final_pos is not None else 0
                
                print(f"모터 {motor_id}: {base_pos} → {final_pos} "
                      f"(이동량: {total_movement} units, {total_movement*360/4096:.1f}°)")
            
            # 최종 오차 통계
            print("\n=== Final Error Statistics ===")
            for motor_id in self.motor_ids:
                valid_errors = [e for e in position_errors[motor_id] if e is not None]
                if valid_errors:
                    avg_error = sum(valid_errors) / len(valid_errors)
                    max_error = max(valid_errors)
                    print(f"Motor {motor_id}: Avg {avg_error:.1f} units ({avg_error*360/4096:.1f}°), Max {max_error:.0f} units ({max_error*360/4096:.1f}°)")
            
        except KeyboardInterrupt:
            print("\n\n상대 위치 동시 애니메이션이 사용자에 의해 중단되었습니다")
        except Exception as e:
            print(f"\n상대 위치 동시 애니메이션 재생 중 오류: {e}")
        
        return times, target_positions, actual_positions, position_errors
    
    def plot_results(self, times, target_positions, actual_positions, position_errors=None):
        """재생 결과 그래프로 표시"""
        try:
            fig, axes = plt.subplots(len(self.motor_ids), 2, figsize=(15, 4*len(self.motor_ids)))
            if len(self.motor_ids) == 1:
                axes = axes.reshape(1, -1)
            
            for i, motor_id in enumerate(self.motor_ids):
                # 위치 그래프
                ax1 = axes[i, 0]
                ax1.plot(times, target_positions[motor_id], 'b-', label=f'Target', linewidth=2)
                
                # 실제 위치 플롯 (None 값 필터링)
                actual_times = []
                actual_pos = []
                for t, pos in zip(times, actual_positions[motor_id]):
                    if pos is not None:
                        actual_times.append(t)
                        actual_pos.append(pos)
                
                if actual_times:
                    ax1.plot(actual_times, actual_pos, 'r-', label=f'Actual', linewidth=1)
                
                ax1.set_xlabel('Time (seconds)')
                ax1.set_ylabel('Position (units)')
                ax1.set_title(f'Motor ID {motor_id} - Simultaneous Relative Position Control')
                ax1.legend()
                ax1.grid(True)
                
                # 두 번째 y축 (각도)
                ax1_deg = ax1.twinx()
                min_pos = min(target_positions[motor_id]) if target_positions[motor_id] else 0
                max_pos = max(target_positions[motor_id]) if target_positions[motor_id] else 4096
                ax1_deg.set_ylim(min_pos * 360 / 4096, max_pos * 360 / 4096)
                ax1_deg.set_ylabel('Angle (degrees)')
                
                # 오차 그래프
                ax2 = axes[i, 1]
                if position_errors and motor_id in position_errors:
                    error_times = []
                    errors = []
                    for t, err in zip(times, position_errors[motor_id]):
                        if err is not None:
                            error_times.append(t)
                            errors.append(err)
                    
                    if error_times:
                        ax2.plot(error_times, errors, 'g-', linewidth=1)
                        ax2.fill_between(error_times, errors, alpha=0.3, color='green')
                        
                        # 평균 오차선
                        avg_error = sum(errors) / len(errors)
                        ax2.axhline(y=avg_error, color='orange', linestyle='--', 
                                   label=f'Avg: {avg_error:.1f} units')
                
                ax2.set_xlabel('Time (seconds)')
                ax2.set_ylabel('Position Error (units)')
                ax2.set_title(f'Motor ID {motor_id} - Position Error')
                ax2.legend()
                ax2.grid(True)
                
                # 두 번째 y축 (각도 오차)
                ax2_deg = ax2.twinx()
                ax2_deg.set_ylabel('Error (degrees)')
                if position_errors and errors:
                    ax2_deg.set_ylim(0, max(errors) * 360 / 4096)
            
            plt.tight_layout()
            plt.show()
            
        except Exception as e:
            print(f"Error plotting results: {e}")


def get_overflow_strategy():
    """속도 초과 처리 전략 선택"""
    strategies = [
        (SpeedOverflowStrategy.REJECT, "재생 거부 - 속도를 줄여서 다시 시도"),
        (SpeedOverflowStrategy.AUTO_SLOW, "자동 속도 감소 - 안전한 속도로 자동 조정"), 
        (SpeedOverflowStrategy.FRAME_SKIP, "프레임 스킵 - 일부 프레임을 건너뛰며 재생"),
        (SpeedOverflowStrategy.INTERPOLATE, "프레임 보간 - 부드러운 움직임으로 보정")
    ]
    
    print("\n=== 속도 초과 시 처리 방법 선택 ===")
    for i, (strategy, description) in enumerate(strategies, 1):
        print(f"{i}. {description}")
    
    while True:
        try:
            choice = int(input(f"\n처리 방법을 선택하세요 (1-{len(strategies)}, default: 2): ") or 2)
            if 1 <= choice <= len(strategies):
                return strategies[choice - 1][0]
            else:
                print(f"1부터 {len(strategies)} 사이의 숫자를 입력하세요")
        except ValueError:
            print("유효한 숫자를 입력하세요")


# 메인 실행 부분
if __name__ == "__main__":
    try:
        print("=== 🚀 다이나믹셀 동시 애니메이션 플레이어 ===")
        
        # 사용자 입력 받기
        port = input("COM 포트 입력 (default: COM3): ") or "COM3"
        
        # 다이나믹셀 컨트롤러 초기화
        controller = MultiJointDynamixelController(port=port)
        
        # 기본 애니메이션 폴더 경로 설정
        animation_folder = "C:/Users/songa/Documents/my_ws/blender_2_motor/animations"
        
        # 애니메이션 파일 입력
        file_input = input(f"애니메이션 파일명 (default: motion3.json): ") or "motion3.json"
        
        if not file_input.endswith('.json'):
            file_input += '.json'
        
        animation_file = os.path.join(animation_folder, file_input)
        
        if not os.path.exists(animation_file):
            print(f"Error: File not found at {animation_file}")
            exit(1)
        
        # ⭐ 동시 제어 플레이어 초기화
        player = TimeBasedSimultaneousPlayer(controller, animation_file)
        
        # 모터 방향 설정
        configure_directions = input("모터 회전 방향을 설정하시겠습니까? (y/n, default: n): ").lower()
        if configure_directions in ['y', 'yes']:
            player.configure_motor_directions()
        
        # 모터 설정 (항상 최고 속도 + 상대 모드 초기화)
        player.setup()
        
        # 재생 속도 설정
        speed = float(input("재생 속도 배율 (default: 1.0): ") or 1.0)
        
        # 속도 초과 처리 전략 선택
        strategy = get_overflow_strategy()
        
        # 안전 확인
        input("Enter를 눌러 상대 위치 동시 애니메이션을 시작합니다 (현재 위치에서 시작)...")
        
        # ⚡ 상대 위치 동시 애니메이션 재생
        times, target, actual, errors = player.play_simultaneous_relative(
            speed_factor=speed, 
            strategy=strategy
        )
        
        # 결과가 있으면 그래프 표시
        if times is not None:
            if input("결과 그래프를 보시겠습니까? (y/n, default: y): ").lower() != 'n':
                player.plot_results(times, target, actual, errors)
        
    except Exception as e:
        print(f"Error: {e}")
    
    finally:
        # 연결 종료
        if 'controller' in locals():
            controller.close()