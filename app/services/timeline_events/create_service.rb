module TimelineEvents
  class CreateService
    def initialize(params, founder, notification_service: Developers::NotificationService.new)
      @params = params
      @founder = founder
      @target = params[:target]
      @notification_service = notification_service
    end

    def execute
      TimelineEvent.transaction do
        TimelineEvent.create!(@params).tap do |te|
          @founder.timeline_event_owners.create!(timeline_event: te, latest: true)
          create_team_entries(te) if @params[:target].team_target?
          @notification_service.execute(
            @founder.course,
            WebhookDelivery.events[:submission_created],
            @founder.user,
            te
          )
          update_latest_flag(te)
        end
      end
    end

    private

    def create_team_entries(timeline_event)
      team_members.each do |member|
        member.timeline_event_owners.create!(timeline_event: timeline_event, latest: true)
      end
    end

    def update_latest_flag(timeline_event)
      old_events = @target.timeline_events.joins(:timeline_event_owners)
        .where(timeline_event_owners: { founder: owners })
        .where.not(id: timeline_event)

      TimelineEventOwner.where(timeline_event_id: old_events, founder: owners).update_all(latest: false) # rubocop:disable Rails/SkipsModelValidations
    end

    def owners
      if @target.team_target?
        @founder.startup.founders
      else
        @founder
      end
    end

    def team_members
      @founder.startup.founders - [@founder]
    end
  end
end
